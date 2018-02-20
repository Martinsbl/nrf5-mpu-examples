 /* 
  * This example is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_uart.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "app_mpu.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}



/* Pins to connect MPU. Pinout is different for nRF51 DK and nRF52 DK
 * and therefore I have added a conditional statement defining different pins
 * for each board. This is only for my own convenience. 
 */
#define MPU_TWI_SCL_PIN 3
#define MPU_TWI_SDA_PIN 4

#define MPU_ADDRESS     		0x68 


/* The buffer length, TWIM_RX_BUF_LENGTH, defines how many samples will fit in the buffer. Each sample
 * may contain accelerometer data and/or gyroscope data and/or temperature. What data to read is defined by 
 * TWIM_RX_BUF_WIDTH and reigster address in p_tx_buffer.*/
#define TWIM_RX_BUF_LENGTH  10


/* The buffer width, TWIM_RX_BUF_WIDTH, and tx buffer, p_tx_buffer, defines how much and what kind of data to read out for every sample. For example: 
 * A buffer width of 6 and register read address MPU_REG_ACCEL_XOUT_H will read out accelerometer data only. 
 * A buffer width of 14 and register read address MPU_REG_ACCEL_XOUT_H will read out all acceleromter, temperateure, and gyroscope data. 
 * A buffer width of 6 and register start address MPU_REG_GYRO_XOUT_H will read out gyroscope data only. */
#define TWIM_RX_BUF_WIDTH   6   // Reading accelerometer only

/* The TWI instance to use to communicate with the MPU */
static const nrf_drv_twi_t m_twi_instance = NRF_DRV_TWI_INSTANCE(0);


/* Define a type with a two dimensioanal array, TWIM_RX_BUF_WIDTH wide and TWIM_RX_BUF_LENGTH long, holding a list of MPU sensor data */
typedef struct ArrayList
{
    uint8_t buffer[TWIM_RX_BUF_WIDTH];
}array_list_t;

/* Declare an RX buffer to hold the sensor data in the MPU we want to read. 
 * TWIM_RX_BUF_LENGTH defines how many samples of accelerometer data and/or gyroscope data and/or temperature
 * data we want to read out. What kind of sensor values we read out is defined by the register address
 * held in p_tx_buffer and the buffer width TWIM_RX_BUF_WIDTH.  */
array_list_t p_rx_buffer[TWIM_RX_BUF_LENGTH];

/* Declare a simple TX buffer holding the first register in MPU we want to read from. */
uint8_t p_tx_buffer[1] = {MPU_REG_ACCEL_XOUT_H};  // Reading accelerometer only

/* Flag to indicate to the applications main context that TWIM_RX_BUF_LENGTH number of samples have been transferred from MPU */
volatile bool twi_transfers_complete = false;

/* Timer instance to trigger each read from MPU */
static nrf_drv_timer_t timer_instance_twi_start = NRF_DRV_TIMER_INSTANCE(0);

/* Counter instance to count number of transferred samples, and trigger an interrupt and wake up CPU 
 * when TWIM_RX_BUF_LENGTH number of samples have transferred from MPU */
static nrf_drv_timer_t twi_transfer_counter_instance = NRF_DRV_TIMER_INSTANCE(1);



// TWI transfer start handler. Not used
void twi_start_timer_handler(nrf_timer_event_t event_type, void * p_context)
{
    ;
}

// TWI transfer counter handler. 
void twi_transfer_counter_handler(nrf_timer_event_t event_type, void * p_context)
{    
    // Set flag to start printing of samples in main loop
    twi_transfers_complete = true;
    
    // Set up next transfer sequence
    nrf_drv_twi_xfer_desc_t xfer = NRF_DRV_TWI_XFER_DESC_TXRX(MPU_ADDRESS, p_tx_buffer, sizeof(p_tx_buffer), (uint8_t*)p_rx_buffer, sizeof(p_rx_buffer) / TWIM_RX_BUF_LENGTH);
    
    // Set flags for next transfer sequence
    uint32_t flags =    NRF_DRV_TWI_FLAG_NO_XFER_EVT_HANDLER    | // We don't use any twi event handlers as we don't want to wake up CPU on TWI events
                        NRF_DRV_TWI_FLAG_HOLD_XFER              | // Don't start transfer yet
                        NRF_DRV_TWI_FLAG_RX_POSTINC             | // Increment address in buffer after each transfer
                        NRF_DRV_TWI_FLAG_REPEATED_XFER;           // Indicate that we are transfering numerous times
    
    // Pass configurations and flags to TWIM driver. The TWIM is now on and prepared, but not actually started.
    uint32_t err_code = nrf_drv_twi_xfer(&m_twi_instance, &xfer, flags);
    APP_ERROR_CHECK(err_code);
}



/**
 * @brief MPU initialization.
 * Just the usual way. Nothing special here
 */
void mpu_init(void)
{
    ret_code_t ret_code;
    // Initiate MPU driver with TWI instance handler
    ret_code = app_mpu_init();
    APP_ERROR_CHECK(ret_code); // Check for errors in return value
    
    // Setup and configure the MPU with intial values
    app_mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG(); // Load default values
    p_mpu_config.smplrt_div = 19;   // Change sampelrate. Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV). 19 gives a sample rate of 50Hz
    p_mpu_config.accel_config.afs_sel = AFS_2G; // Set accelerometer full scale range to 2G
    ret_code = app_mpu_config(&p_mpu_config); // Configure the MPU with above values
    APP_ERROR_CHECK(ret_code); // Check for errors in return value 
}

/**
 * @brief TWI transfer setup. This functions initiates the timer used to trigger
 * the TWI transfers. It also configures the the TWI transfers and sets up a PPI channel 
 * connecting the timer compare event with the TWI transfer start task. 
 */
static void twi_transfer_start_timer_init()
{
    ret_code_t err_code;
    // Variable to store address of timer compare event. 
    uint32_t event_address_timer_compare;
    // Variable to store address of twi start task.
    uint32_t task_address_twi_start;
    // Variable holding PPI channel number
    nrf_ppi_channel_t ppi_channel_twi_start;
    
    // Set up next transfer sequence
    nrf_drv_twi_xfer_desc_t xfer = NRF_DRV_TWI_XFER_DESC_TXRX(MPU_ADDRESS, p_tx_buffer, sizeof(p_tx_buffer), (uint8_t*)p_rx_buffer, sizeof(p_rx_buffer) / TWIM_RX_BUF_LENGTH);
    
    // Set flags for next transfer sequence
    uint32_t flags =    NRF_DRV_TWI_FLAG_NO_XFER_EVT_HANDLER    | // We don't use any twi event handlers as we don't want to wake up CPU on TWI events
                        NRF_DRV_TWI_FLAG_HOLD_XFER              | // Don't start transfer yet
                        NRF_DRV_TWI_FLAG_RX_POSTINC             | // Increment address in buffer after each transfer
                        NRF_DRV_TWI_FLAG_REPEATED_XFER;           // Indicate that we are transfering numerous times
    
    // Pass configurations and flags to TWIM driver. The TWIM is now on and prepared, but not actually started.
    err_code = nrf_drv_twi_xfer(&m_twi_instance, &xfer, flags);
    APP_ERROR_CHECK(err_code);
    
    // Initializing the timer triggering the TWI transfers. Passing:
    // Instance of timer
    // NULL = default timer configurations
    // Timer handler. Can not be NULL even though it is not used.
    err_code = nrf_drv_timer_init(&timer_instance_twi_start, NULL, twi_start_timer_handler);
    APP_ERROR_CHECK(err_code);
    
    uint32_t mpu_sample_period = nrf_drv_timer_us_to_ticks(&timer_instance_twi_start, 20000); // Sample every 20 ms (50 Hz)
    // Configuring the timer triggering the TWI transfers. Passing:
    // Timer instance
    // Capture Compare channel number
    // Sample period in micro seconds
    // Shortcut to clear timer on Capture Compare event
    // Disable the interrupt for the compare channel enabling the CPU to sleep during events
    nrf_drv_timer_extended_compare(&timer_instance_twi_start, (nrf_timer_cc_channel_t)0, mpu_sample_period, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
    

    // Initate ppi driver. Ignore error if driver is already initialized
    err_code = nrf_drv_ppi_init();
    if((err_code != 0) && (err_code != NRF_ERROR_MODULE_ALREADY_INITIALIZED))
    {
        APP_ERROR_CHECK(err_code);
    }
    
    // Get the address of the TWI start task. This is the register address of the actual TWI task. Look in the nRF52 Product Specification -> TWI -> Registers
    task_address_twi_start = nrf_drv_twi_start_task_get(&m_twi_instance, xfer.type);
    // Get the address of the timer compare[0] event. This is the register address of the actual timer compare[0] event. Look in the nRF52 Product Specification -> Timer -> Registers
    event_address_timer_compare = nrf_drv_timer_event_address_get(&timer_instance_twi_start, NRF_TIMER_EVENT_COMPARE0);
    
    // Allocate a PPI channel. This function is especially useful when using a softdevice as it will ensure that the the returend PPI channels is free
    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel_twi_start);
    APP_ERROR_CHECK(err_code);
    
    // Connect the timer compare event and twi start task using the PPI channel
    err_code = nrf_drv_ppi_channel_assign(ppi_channel_twi_start, event_address_timer_compare, task_address_twi_start);
    APP_ERROR_CHECK(err_code);

    // Enable the PPI channel.
    err_code = nrf_drv_ppi_channel_enable(ppi_channel_twi_start);
    APP_ERROR_CHECK(err_code);
       
}

/**
 * @brief This function sets up a counter used to count TWI transfers. It also sets up a PPI channel 
 * connecting the Counter increment task and the TWI transfer complete event. 
 * When each TWI transfer is completed the counter increments. When TWIM_RX_BUF_LENGTH number of samples is counted
 * the counter triggers an interrupt, twi_transfer_counter_handler, and wakes up the CPU.
 */
void twi_transfer_counter_init(void)
{
    // Setup TWI tranfer counter
    uint32_t err_code;
    // Variable to store address of counter increment task. 
    uint32_t task_address_counter_increment;
    // Variable to store address of TWI transfer complete event. 
    uint32_t event_address_twi_transfer_complete;
    // Variable holding PPI channel number
    nrf_ppi_channel_t ppi_channel_twi_transfer_count;
    
    // Set up counter with default configuration
    nrf_drv_timer_config_t counter_config = NRF_DRV_TIMER_DEFAULT_CONFIG;
    counter_config.mode = NRF_TIMER_MODE_COUNTER;
    
    // Initializing the counter counting the TWI transfers. Passing:
    // Instance of counter
    // Configurations
    // Timer handler. This timer will be triggered when TWIM_RX_BUF_LENGTH number of transfers have completed.
    err_code = nrf_drv_timer_init(&twi_transfer_counter_instance, &counter_config, twi_transfer_counter_handler);
    APP_ERROR_CHECK(err_code);
    
    // Configuring the timer triggering the TWI transfers. Passing:
    // Counter instance
    // Capture Compare channel number
    // Number of samples to count
    // Shortcut to clear timer on Capture Compare event
    // Use interrupts
    nrf_drv_timer_extended_compare(&twi_transfer_counter_instance, (nrf_timer_cc_channel_t)0, TWIM_RX_BUF_LENGTH, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
    
    // Initate ppi driver. Ignore error if driver is already initialized
    err_code = nrf_drv_ppi_init();
    if((err_code != 0) && (err_code != NRF_ERROR_MODULE_ALREADY_INITIALIZED))
    {
        APP_ERROR_CHECK(err_code);
    }
    
    // Allocate a PPI channel. This function is especially useful when using a softdevice as it will ensure that the the returend PPI channels is free
    err_code = nrf_drv_ppi_channel_alloc(&ppi_channel_twi_transfer_count);
    APP_ERROR_CHECK(err_code);
    
    // Get the address of the counter increment task. This is the register address of the actual counter task. Look in the nRF52 Product Specification -> Timer -> Registers
    task_address_counter_increment = nrf_drv_timer_task_address_get(&twi_transfer_counter_instance, NRF_TIMER_TASK_COUNT);
    // Get the address of the TWI transfer complete event. This is the register address of the event. Look in the nRF52 Product Specification -> TWI -> Registers
    event_address_twi_transfer_complete = nrf_drv_twi_stopped_event_get(&m_twi_instance);
    
    // Connect the TWI transfer complete event and the counter increment task using the PPI channel
    err_code = nrf_drv_ppi_channel_assign(ppi_channel_twi_transfer_count, event_address_twi_transfer_complete, task_address_counter_increment);
    APP_ERROR_CHECK(err_code);

    // Enable the PPI channel.
    err_code = nrf_drv_ppi_channel_enable(ppi_channel_twi_transfer_count);
    APP_ERROR_CHECK(err_code);
}


void twi_transfer_start()
{
    // Enable the counter counting number of TWI transfers
    nrf_drv_timer_enable(&twi_transfer_counter_instance);
    
    // Enable timer triggering TWI transfers
    nrf_drv_timer_enable(&timer_instance_twi_start);
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    // Configure some LEDs
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
    
    // Initialize.
    log_init();
	NRF_LOG_INFO("\033[2J\033[;H"); // Clear screen
    
    // Setup the MPU
    mpu_init();
    
    // Start execution.
    NRF_LOG_INFO("\033[2J\033[;HMPU nRF52 EasyDMA using timers and drivers example. Compiled @ %s\r\n", __TIME__);
    
    
    // Configure a timer to trigger TWI transfers between nRF52 and MPU
    twi_transfer_start_timer_init();   
    // Configure a counter to count number of TWI transfers
    twi_transfer_counter_init();
    // Start the TWI transfers between the nRF52 and MPU
    twi_transfer_start();
    
    
    uint32_t sample_nr = 0; // Variable holding number of samples read from MPU
    accel_values_t acc_values; // Variable to temporarily hold MPU accelerometer data
    
    while(1)
    {
        if(NRF_LOG_PROCESS() == false)
        {
            nrf_gpio_pin_set(LED_4); // Turn LED OFF when CPU is sleeping
            while(twi_transfers_complete == false)
            {
                // Make sure any pending events are cleared
                __SEV();
                __WFE();
                // Enter System ON sleep mode
                __WFE();           
            }
            nrf_gpio_pin_clear(LED_4); // Turn LED ON when CPU is working
            
            // Clear terminal
            NRF_LOG_RAW_INFO("\033[3;1HSample %d:\r\n", (TWIM_RX_BUF_LENGTH * ++sample_nr));
            
            // THIS FOR LOOP ASSUMES THAT TWIM_RX_BUF_WIDTH IS 6 BYTES AND THAT ONLY ACCELEROMETER DATA IS SAMPLED
            // IF A WIDER BUFFER IS USED TO SAMPLE TEMPERATURE AND GYROSCOPE AS WELL YOU SHOULD CHANGE THIS LOOP
            // TO PRINT EVERYTHING
            uint8_t *data;
            // Itterate through entire RX buffer 
            for(uint8_t j = 0; j<TWIM_RX_BUF_LENGTH; j++)
            {
                // Temporarily store each sensor data set found in buffer in accelerometer structure variable
                data = (uint8_t*)&acc_values;
                // Itterate through and store all data in each sensor set
                for(uint8_t i = 0; i<6; i++)
                {
                    *data = p_rx_buffer[j].buffer[5-i];
                    data++;
                }
                // Print sensor data set
                NRF_LOG_RAW_INFO("X %06d\r\nY %06d\r\nZ %06d\r\n\r\n", (int16_t)acc_values.x, (int16_t)acc_values.y, (int16_t)acc_values.z);
                nrf_delay_ms(1); // Small delay so not to overload the UART 
            }
            // Reset data ready flag
            twi_transfers_complete = false;
        }
    }
}

/** @} */
