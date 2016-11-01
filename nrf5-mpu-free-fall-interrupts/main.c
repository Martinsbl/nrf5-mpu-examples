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
#include "nrf_delay.h"
#include "app_mpu.h"
#include "nrf_drv_gpiote.h"

#if defined(MPU9255) || defined(MPU60x0) 
    #error "This example does not work on MPU60x0 and MPU9255!"
#endif

/*Pins to connect MPU. */
#define MPU_MPU_INT_PIN     3

/*UART buffer size. */
#define UART_TX_BUF_SIZE 256
#define UART_RX_BUF_SIZE 1

volatile bool mpu_free_fall_event = false;

/**
 * @brief UART events handler.
 */
static void uart_events_handler(app_uart_evt_t * p_event)
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
 * Just the usual way. Nothing special here
 */
static void uart_config(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
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


void mpu_setup(void)
{
    uint32_t err_code;
    // Initiate MPU driver
    err_code = mpu_init();
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    // Setup and configure the MPU with intial values
    mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG(); // Load default values
    p_mpu_config.smplrt_div = 9;   // Change sampelrate. Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV). 19 gives a sample rate of 50Hz
    p_mpu_config.accel_config.afs_sel = AFS_16G; // Set accelerometer full scale range to 2G
    err_code = mpu_config(&p_mpu_config); // Configure the MPU with above values
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    
    // This is a way to configure the interrupt pin behaviour
    mpu_int_pin_cfg_t p_int_pin_cfg = MPU_DEFAULT_INT_PIN_CONFIG(); // Default configurations
    p_int_pin_cfg.int_rd_clear = 1; // When this bit is equal to 1, interrupt status bits are cleared on any read operation
    err_code = mpu_int_cfg_pin(&p_int_pin_cfg); // Configure pin behaviour
    APP_ERROR_CHECK(err_code); // Check for errors in return value
    
    mpu_config_ff_detection(900, 2);
    
    // Enable the MPU interrupts
    mpu_int_enable_t p_int_enable = MPU_DEFAULT_INT_ENABLE_CONFIG();
    p_int_enable.ff_en = 1; // Trigger interrupt on free fall
    err_code = mpu_int_enable(&p_int_enable); // Configure interrupts
    APP_ERROR_CHECK(err_code); // Check for errors in return value    
}

/**
 * @brief Simple interrupt handler setting a flag indicating a free fall
 *
 */
void int_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    mpu_free_fall_event = true;
}

/**
 * @brief Function for initiating the GPIOTE module and enable the 
 * nRF5 to trigger an interrupt on a Low-To-High event on pin MPU_MPU_INT_PIN
 *
 */
static void gpiote_init(void)
{
    uint32_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

    err_code = nrf_drv_gpiote_in_init(MPU_MPU_INT_PIN, &in_config, int_pin_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(MPU_MPU_INT_PIN, true);
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    uint32_t err_code;
    nrf_gpio_range_cfg_output(LED_START, LED_STOP);
    uart_config();
    printf("\033[2J\033[;HMPU example with MPU generated free fall interrupts. Compiled @ %s\r\n", __TIME__);
    gpiote_init();
    mpu_setup();
    
    accel_values_t acc_values;
    uint32_t sample_number = 0;
        
    while(1)
    {
        nrf_gpio_pin_set(LED_4); // LED off when CPU is sleeping
        while(mpu_free_fall_event != true)
        {
            // Make sure any pending events are cleared
            __SEV();
            __WFE();
            // Enter System ON sleep mode
            __WFE();           
        }
        nrf_gpio_pin_clear(LED_4); // Light LED when CPU is working
        
        err_code = mpu_read_accel(&acc_values);
        APP_ERROR_CHECK(err_code);
        // Clear terminal and print values
        printf("\033[2J\033[;HFree fall # %d\r\nX: %06d\r\nY: %06d\r\nZ: %06d", ++sample_number, acc_values.x, acc_values.y, acc_values.z);
        mpu_free_fall_event = false;
    }
}



/** @} */
