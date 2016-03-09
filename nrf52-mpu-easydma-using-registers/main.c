
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "nrf_delay.h"
#include "app_uart.h"
#include "bsp.h"
#include "app_error.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "mpu_register_map.h"
#include "mpu.h"

// UART buffer sizes. 
#define UART_TX_BUF_SIZE 256 
#define UART_RX_BUF_SIZE 1   

// Pins to connect MPU. 
#define MPU_TWI_SCL_PIN 3
#define MPU_TWI_SDA_PIN 4
#define MPU_INT_PIN 28

// TWI buffer sizes. 
#define TWIM_TX_BUF_SIZE    1
#define TWIM_RX_BUF_LENGTH  10 // Number of sample sets to read from MPU
#define TWIM_RX_BUF_WIDTH   6 // Adjust for what data you want to read out from MPU. E.g. 6 bytes reading from MPU_REG_ACCEL_XOUT_H reads out all accelerometer data

// The TWI instance to use to communicate with the MPU 
static const nrf_drv_twi_t m_twi_instance = NRF_DRV_TWI_INSTANCE(0);

// Define a type with a two dimensioanal array holding a list of MPU sensor data 
typedef struct ArrayList
{
    uint8_t buffer[TWIM_RX_BUF_WIDTH]; // 6 bytes wide to fit just accelerometer data
}array_list_t;

// Declare a MPU sensor data buffer 
array_list_t p_rx_buffer[TWIM_RX_BUF_LENGTH];
// Declare a simple TX buffer holding the firs register in MPU we want to read from. 
uint8_t p_tx_buffer[TWIM_TX_BUF_SIZE] = {MPU_REG_ACCEL_XOUT_H};

// Flag used to indicate to the applications main context that new sensor data is available 
volatile bool twi_transfers_complete = false;


/*
 * @brief UART events handler. Not really necessary for this example
 */
void uart_events_handler(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
    {
        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

/**
 * @brief UART initialization.
 */
void uart_config(void)
{
    // Standard UART setup with fast baudrate so we are able to print everything rapidly 
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_ENABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };
    
    APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_events_handler,
                       APP_IRQ_PRIORITY_LOW,
                       err_code);

    APP_ERROR_CHECK(err_code);
}

/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{   
    // Pass TWI events down to the MPU driver. 
    mpu_twi_event_handler(p_event);
}


/**
 * @brief Initialize the TWI Master module with PPI triggered
 * R/W operations started by a counter module
 */
static void twi_with_easy_dma_setup()
{
    // Disable the TWIM module while we reconfigure it
    NRF_TWIM0->ENABLE = TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos;
    
    // Configure a gpiote channel to generate an event on a polarity change from 
    // low to high generated the MPU interrupt pin.
    uint8_t gpiote_ch_mpu_int_event = 0;
    NRF_GPIOTE->CONFIG[gpiote_ch_mpu_int_event] = ( (GPIOTE_CONFIG_MODE_Event   << GPIOTE_CONFIG_MODE_Pos) | 
                                                    (MPU_INT_PIN                << GPIOTE_CONFIG_PSEL_Pos) | 
                                                    (GPIOTE_CONFIG_POLARITY_LoToHi << GPIOTE_CONFIG_POLARITY_Pos));
    
    // Load TWI TX buffer into TWI module. Set number of bytes to write pr transfer, max count, to one. 
    // Disable the EasyDMA list functionality for TWI TX.
    NRF_TWIM0->TXD.PTR = (uint32_t)&p_tx_buffer;
    NRF_TWIM0->TXD.MAXCNT = TWIM_TX_BUF_SIZE;
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
void counter_init()
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
    
    // Enable timer interrupt
    NVIC_EnableIRQ(TIMER0_IRQn);
    // Start timer
    
}

/**
 * @brief Start the transfers
 */
void start_transfers(void)
{
    // Start counter
    NRF_TIMER0->TASKS_START = 1;
    // Enable the PPI channel tying MPU interrupt pin to TWIM module
    NRF_PPI->CHEN = PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos;
}

/**
 * @brief Timer event hadnler triggered on counter compare, i.e. everytime
 * TWI RX buffer is full
 */
void TIMER0_IRQHandler(void)
{
    // Clear timer event
    NRF_TIMER0->EVENTS_COMPARE[0] = 0;
    // Reset the TWIM RX pointer to initial address of RX buffer
    NRF_TWIM0->RXD.PTR = (uint32_t)&p_rx_buffer;
    // Toggle a LED for show
    nrf_drv_gpiote_out_toggle(LED_1);
    // Set flag to notify main context of the new data available
    twi_transfers_complete = true;  
}

/**
 * @brief TWI initialization.
 * Just the usual way. Nothing special here
 */
void twi_init(void)
{
    ret_code_t err_code;
    
    const nrf_drv_twi_config_t twi_mpu_config = {
       .scl                = MPU_TWI_SCL_PIN,
       .sda                = MPU_TWI_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH
    };
    
    err_code = nrf_drv_twi_init(&m_twi_instance, &twi_mpu_config, twi_handler, NULL);
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_twi_enable(&m_twi_instance);
}

void mpu_setup()
{
    uint32_t err_code;
    
    // MPU9150 setup
    err_code = mpu_init(&m_twi_instance);
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG();
    p_mpu_config.smplrt_div = 19;
    p_mpu_config.accel_config.afs_sel = AFS_2G;
    err_code = mpu_config(&p_mpu_config);
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    mpu_int_pin_cfg_t p_int_pin_cfg = MPU_DEFAULT_INT_PIN_CONFIG();
    p_int_pin_cfg.int_rd_clear = 1; // Read operation will clear the MPU interrupt
    err_code = mpu_int_cfg_pin(&p_int_pin_cfg);
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    mpu_int_enable_t p_int_enable = MPU_DEFAULT_INT_ENABLE_CONFIG();
    p_int_enable.data_rdy_en = 1; // Enable interrupt on completed sample
    err_code = mpu_int_enable(&p_int_enable);
    APP_ERROR_CHECK(err_code); // Check for errors in return value
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    nrf_gpio_range_cfg_output(LED_1, LED_1);
    // Initate UART and pring welcome message
    uart_config();
    printf("\033[2J\033[;HMPU nRF52 EasyDMA register example. Compiled @ %s\r\n", __TIME__);
    
    // Initiate TWI
    twi_init();
    // Initiate the MPU with interrupt
    mpu_setup();
    
    // Initiate counter to count number of TWI transfers 
    counter_init();
    // Reconfigure TWI to use PPI and easyDMA
    twi_with_easy_dma_setup();
    
    // Starting the transfers
    start_transfers();
    
    // Declare a value to count number of samples recevied
    uint8_t sample_nr = 0;
    // Accelerometer structure to hold new values.
    accel_values_t acc_values;
   
    
    while (true)
    {
        // Wait for new available data 
        while(twi_transfers_complete == false){}
            
        // Print header with total number of samples received
        printf("\033[3;1HSample %d:\r\n", TWIM_RX_BUF_LENGTH * sample_nr++);
        // Declare pointer used to point to RX buffer
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
            printf("X %06d\r\nY %06d\r\nZ %06d\r\n\r\n", (int16_t)acc_values.x, (int16_t)acc_values.y, (int16_t)acc_values.z);
            // Small delay so not to overload the UART 
            nrf_delay_ms(1); 
        }
        // Reset data ready flag
        twi_transfers_complete = false;
         
        // Enter System ON sleep mode
        __WFE();
        // Make sure any pending events are cleared
        __SEV();
        __WFE();
    }
}
/** @} */
