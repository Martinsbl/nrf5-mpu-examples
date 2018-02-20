 /* 
  * This example is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "nrf_delay.h"
#include "app_uart.h"
#include "bsp.h"
#include "app_error.h"
#include "app_mpu.h"


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
#if defined(BOARD_PCA10040)
#define MPU_TWI_SCL_PIN 3
#define MPU_TWI_SDA_PIN 4
#define MPU_INT_PIN     30
#else // Pins for PCA10028
#define MPU_TWI_SCL_PIN 1
#define MPU_TWI_SDA_PIN 2
#define MPU_INT_PIN     5
#endif


#define MPU_ADDRESS     		0x68 

/* The buffer length, TWIM_RX_BUF_LENGTH, defines how many samples will fit in the buffer. Each sample
 * may contain accelerometer data and/or gyroscope data and/or temperature. What data to read is defined by 
 * TWIM_RX_BUF_WIDTH and reigster address in p_tx_data.*/
#define TWIM_RX_BUF_LENGTH  10


/* The buffer width, TWIM_RX_BUF_WIDTH, and tx buffer, p_tx_data, defines how much and what kind of data to read out for every sample. For example: 
 * A buffer width of 6 and register read address MPU_REG_ACCEL_XOUT_H will read out accelerometer data only. 
 * A buffer width of 14 and register read address MPU_REG_ACCEL_XOUT_H will read out all acceleromter, temperateure, and gyroscope data. 
 * A buffer width of 6 and register start address MPU_REG_GYRO_XOUT_H will read out gyroscope data only. */
#define TWIM_RX_BUF_WIDTH   6   // Reading accelerometer only


/* Define a type with a two dimensioanal array, TWIM_RX_BUF_WIDTH wide and TWIM_RX_BUF_LENGTH long, holding a list of MPU sensor data */
typedef struct ArrayList
{
    uint8_t buffer[TWIM_RX_BUF_WIDTH];
}array_list_t;

/* Declare an RX buffer to hold the sensor data in the MPU we want to read. 
 * TWIM_RX_BUF_LENGTH defines how many samples of accelerometer data and/or gyroscope data and/or temperature
 * data we want to read out. What kind of sensor values we read out is defined by the register address
 * held in p_tx_data and the buffer width TWIM_RX_BUF_WIDTH.  */
array_list_t p_rx_buffer[TWIM_RX_BUF_LENGTH];

/* Declare a simple TX buffer holding the first register in MPU we want to read from. */
uint8_t p_tx_buffer[1] = {MPU_REG_ACCEL_XOUT_H};  // Reading accelerometer only

/* Flag to indicate to the applications main context that TWIM_RX_BUF_LENGTH number of samples have been transferred from MPU */
volatile bool twi_transfers_complete = false;



/**
 * @brief Initialize the TWI Master module with PPI triggered
 * R/W operations started by a counter module
 */
static void twi_with_easy_dma_setup()
{
    // Disable the TWIM module while we reconfigure it
    NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
    NRF_TWIM0->SHORTS = 0;
    NVIC_DisableIRQ(SPI0_TWI0_IRQn);
    NVIC_ClearPendingIRQ(SPI0_TWI0_IRQn);
    
    // Configure a gpiote channel to generate an event on a polarity change from 
    // low to high generated the MPU interrupt pin.
    uint8_t gpiote_ch_mpu_int_event = 0;
    NRF_GPIOTE->CONFIG[gpiote_ch_mpu_int_event] = ( (GPIOTE_CONFIG_MODE_Event   << GPIOTE_CONFIG_MODE_Pos) | 
                                                    (MPU_INT_PIN                << GPIOTE_CONFIG_PSEL_Pos) | 
                                                    (GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos));
    
    NRF_TWIM0->PSEL.SCL = MPU_TWI_SCL_PIN;
    NRF_TWIM0->PSEL.SDA = MPU_TWI_SDA_PIN;
    NRF_TWIM0->FREQUENCY = TWI_FREQUENCY_FREQUENCY_K400;
    
    // Load TWI TX buffer into TWI module. Set number of bytes to write pr transfer, max count, to one. 
    // Disable the EasyDMA list functionality for TWI TX.
    NRF_TWIM0->TXD.PTR = (uint32_t)&p_tx_buffer;
    NRF_TWIM0->TXD.MAXCNT = 1;
    NRF_TWIM0->TXD.LIST = TWIM_TXD_LIST_LIST_Disabled << TWIM_TXD_LIST_LIST_Pos;
    
    // Point to TWI RX buffer. Set number of bytes to read pr transfer, max count, to TWIM_RX_BUF_WIDTH. 
    // Disable the EasyDMA list functionality for TWI TX
    NRF_TWIM0->RXD.PTR = (uint32_t)&p_rx_buffer;
    NRF_TWIM0->RXD.MAXCNT = TWIM_RX_BUF_WIDTH;
    NRF_TWIM0->RXD.LIST = TWIM_RXD_LIST_LIST_ArrayList << TWIM_RXD_LIST_LIST_Pos;
    
    // Make sure that MPU address is set
    NRF_TWIM0->ADDRESS = MPU_ADDRESS;
    // Enable shortcuts that starts a read right after a write and sends a stop condition after last TWI read
    NRF_TWIM0->SHORTS = (TWIM_SHORTS_LASTTX_STARTRX_Enabled << TWIM_SHORTS_LASTTX_STARTRX_Pos) | 
                        (TWIM_SHORTS_LASTRX_STOP_Enabled << TWIM_SHORTS_LASTRX_STOP_Pos);
    
    // Configure PPI channel
    // Use MPU interrupt pin as event
    // Start timer 0 on event to count number of transfers
    // Also start TWI transfers on event
    // Enable PPI channel
    NRF_PPI->CH[0].EEP = (uint32_t)&NRF_GPIOTE->EVENTS_IN[gpiote_ch_mpu_int_event];
    NRF_PPI->CH[0].TEP = (uint32_t)&NRF_TIMER0->TASKS_COUNT;
    NRF_PPI->FORK[0].TEP = (uint32_t)&NRF_TWIM0->TASKS_STARTTX;
    
    // Enable the TWIM module
    NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos;
}


/**
 * @brief Initialize the counter in Timer 0 used to count number of 
 * TWI transfers from MPU 
 */
void twi_transfer_counter_init()
{
    // Disable and clear any pending Timer 0 interrupts
    NVIC_DisableIRQ(TIMER0_IRQn);
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    
    // Stop timer if running
    NRF_TIMER0->TASKS_STOP = 1;
    // Enabel shortcut to clear the compare register on a compare event
    NRF_TIMER0->SHORTS  = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
    // Use timer 0 as counter
    NRF_TIMER0->MODE    = TIMER_MODE_MODE_Counter << TIMER_MODE_MODE_Pos;
    // Set bit mode to 8 bit.
    NRF_TIMER0->BITMODE = TIMER_BITMODE_BITMODE_08Bit << TIMER_BITMODE_BITMODE_Pos;
    // Set compare register to length of RX buffer. This will trigger an event each time
    // the RX buffer is full
    NRF_TIMER0->CC[0]   = TWIM_RX_BUF_LENGTH;
    // Enable interrupts on counter compare
    NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
    
}

/**
 * @brief Start the transfers
 */
void start_transfers(void)
{
    // Enable timer interrupt
    NVIC_EnableIRQ(TIMER0_IRQn);
    // Start counter
    NRF_TIMER0->TASKS_START = 1;
    // Enable the PPI channel tying MPU interrupt pin to TWIM module
    NRF_PPI->CHEN = PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos;
}

/**
 * @brief Timer event handler triggered on counter compare, i.e. everytime
 * TWI RX buffer is full
 */
void TIMER0_IRQHandler(void)
{
    // Clear timer event
    NRF_TIMER0->EVENTS_COMPARE[0] = 0;
    // Reset the TWIM RX pointer to initial address of RX buffer
    NRF_TWIM0->RXD.PTR = (uint32_t)&p_rx_buffer;
    // Toggle a LED for show
    nrf_gpio_pin_toggle(LED_1);
    // Set flag to notify main context of the new data available
    twi_transfers_complete = true;  
}


/**
 * @brief MPU initialization.
 * Just the usual way. Nothing special here
 */
void mpu_init()
{
    uint32_t err_code;
    
    // MPU setup
    err_code = app_mpu_init();
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    app_mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG();
    p_mpu_config.smplrt_div = 19;
    p_mpu_config.accel_config.afs_sel = AFS_2G;
    err_code = app_mpu_config(&p_mpu_config);
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    app_mpu_int_pin_cfg_t p_int_pin_cfg = MPU_DEFAULT_INT_PIN_CONFIG();
    p_int_pin_cfg.int_rd_clear = 1; // Read operation will clear the MPU interrupt
    err_code = app_mpu_int_cfg_pin(&p_int_pin_cfg);
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    app_mpu_int_enable_t p_int_enable = MPU_DEFAULT_INT_ENABLE_CONFIG();
    p_int_enable.data_rdy_en = 1; // Enable interrupt on completed sample
    err_code = app_mpu_int_enable(&p_int_enable);
    APP_ERROR_CHECK(err_code); // Check for errors in return value
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
    
    // Initialize.
    log_init();
	NRF_LOG_INFO("\033[2J\033[;H"); // Clear screen
    
    mpu_init();
    
    // Start execution.
    NRF_LOG_INFO("MPU EasyDMA using GPIOTE and registers example.");
    
    
    // Initiate counter to count number of TWI transfers 
    twi_transfer_counter_init();
    // Reconfigure TWI to use PPI and easyDMA
    twi_with_easy_dma_setup();
    
    // Starting the transfers
    start_transfers();
    
    uint32_t sample_nr = 0; // Variable holding number of samples read from MPU
    accel_values_t acc_values; // Variable to temporarily hold MPU accelerometer data
    
    while (true)
    {
        if(NRF_LOG_PROCESS() == false)
        {
            nrf_gpio_pin_set(LED_4); // Turn LED OFF when CPU is sleeping
            // Wait for new available data 
            while(twi_transfers_complete == false)
            {
                // Make sure any pending events are cleared
                __SEV();
                __WFE();
                // Enter System ON sleep mode
                __WFE();           
            }
            nrf_gpio_pin_clear(LED_4); // Turn LED ON when CPU is working
            // Print header with total number of samples received
            NRF_LOG_RAW_INFO("\033[3;1HSample %d:\r\n", TWIM_RX_BUF_LENGTH * sample_nr++);
            
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
                for(uint8_t i = 0; i<TWIM_RX_BUF_WIDTH; i++)
                {
                    *data = p_rx_buffer[j].buffer[5-i];
                    data++;
                }
                // Print sensor data set
                NRF_LOG_RAW_INFO("X %06d\r\nY %06d\r\nZ %06d\r\n\r\n", (int16_t)acc_values.x, (int16_t)acc_values.y, (int16_t)acc_values.z);
                // Small delay so not to overload the UART 
                nrf_delay_ms(2); 
            }
            // Reset data ready flag
            twi_transfers_complete = false;
        }
    }
}
/** @} */
