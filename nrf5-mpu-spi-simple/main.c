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
#include "nrf_drv_spi.h"
#include "nrf_delay.h"
#include "drv_mpu_spi.h"

/* Pins to connect MPU. Pinout is different for nRF51 and nRF52 DK */
#define MPU_SPI_MISO_PIN    28 // MPU SDO
#define MPU_SPI_MOSI_PIN    4  // MPU SDI
#define MPU_SPI_SCL_PIN     3  // MPU SCLK
#define MPU_SPI_CS_PIN      29 // MPU nCS

/*UART buffer size. */
#define UART_TX_BUF_SIZE 256
#define UART_RX_BUF_SIZE 1
static const nrf_drv_spi_t m_spi_instance = NRF_DRV_SPI_INSTANCE(0);

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


/**
 * @brief SPI events handler.
 */
void spi_handler(nrf_drv_spi_evt_t const * p_event)
{   
    // Pass SPI events down to the MPU driver.
    mpu_spi_event_handler(p_event);
}

/**
 * @brief SPI initialization.
 * Just the usual way. Nothing special here
 */
void spi_init(void)
{
    ret_code_t err_code;
    
    const nrf_drv_spi_config_t spi_mpu_config = {                                                            \
        .sck_pin      = MPU_SPI_SCL_PIN, 
        .mosi_pin     = MPU_SPI_MOSI_PIN,
        .miso_pin     = MPU_SPI_MISO_PIN,
        .ss_pin       = MPU_SPI_CS_PIN,
        .irq_priority = APP_IRQ_PRIORITY_HIGH, 
        .orc          = 0xFF,                                    
        .frequency    = NRF_DRV_SPI_FREQ_1M,                     
        .mode         = NRF_DRV_SPI_MODE_0,                      
        .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST,         
    };
    
    err_code = nrf_drv_spi_init(&m_spi_instance, &spi_mpu_config, spi_handler);
    APP_ERROR_CHECK(err_code);
}

void mpu_setup(void)
{
    ret_code_t ret_code;
    // Initiate MPU driver with SPI instance handler
    ret_code = mpu_init(&m_spi_instance);
    APP_ERROR_CHECK(ret_code); // Check for errors in return value
    
    // Setup and configure the MPU with intial values
    mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG(); // Load default values
    p_mpu_config.smplrt_div = 19;   // Change sampelrate. Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV). 19 gives a sample rate of 50Hz
    p_mpu_config.accel_config.afs_sel = AFS_2G; // Set accelerometer full scale range to 2G
    ret_code = mpu_config(&p_mpu_config); // Configure the MPU with above values
    APP_ERROR_CHECK(ret_code); // Check for errors in return value 
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{    
    uint32_t err_code;
    uart_config();
    printf("\033[2J\033[;HMPU SPI simple example. Compiled @ %s\r\n", __TIME__);
    spi_init();
    mpu_setup();
    
    
    
    accel_values_t acc_values;
    uint32_t sample_number = 0;
    while(1)
    {
        // Read accelerometer sensor values
        err_code = mpu_read_accel(&acc_values);
        APP_ERROR_CHECK(err_code);
        // Clear terminal and print values
        printf("\033[3;1HSample # %d\r\nX: %06d\r\nY: %06d\r\nZ: %06d", ++sample_number, acc_values.x, acc_values.y, acc_values.z);
        nrf_delay_ms(250);
    }
}

/** @} */
