 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */
  
#if defined(MPU_USES_SPI) // Use SPI drivers

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_drv_spi.h"
#include "nrf_drv_mpu.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"

#if defined(MPU9150)
#error "MPU9150 does not support SPI"
#endif

/* Pins to connect MPU. Pinout is different for nRF51 DK and nRF52 DK
 * and therefore I have added a conditional statement defining different pins
 * for each board. This is only for my own convenience and might be subject to changes as I devleop. 
 */
#define MPU_SPI_MISO_PIN    28 // MPU SDO. 'AD0' on MPU breakout board silk screen
#define MPU_SPI_MOSI_PIN    4  // MPU SDI. 'SDA' on MPU breakout board silk screen
#define MPU_SPI_SCL_PIN     3  // MPU SCLK. 'SCL' on MPU breakout board silk screen
#define MPU_SPI_CS_PIN      29 // MPU nCS. 'NCS' on MPU breakout board silk screen


#define MPU_SPI_BUFFER_SIZE     14 // 14 byte buffers will suffice to read acceleromter, gyroscope and temperature data in one transmission.
#define MPU_SPI_WRITE_BIT       0x00
#define MPU_SPI_READ_BIT        0x80
#define MPU_SPI_TIMEOUT         5000 


static const nrf_drv_spi_t m_spi_instance = NRF_DRV_SPI_INSTANCE(0);
volatile static bool spi_tx_done = false;


uint8_t spi_tx_buffer[MPU_SPI_BUFFER_SIZE];
uint8_t spi_rx_buffer[MPU_SPI_BUFFER_SIZE];


void nrf_drv_mpu_spi_event_handler(const nrf_drv_spi_evt_t *evt, void * p_context)
{
    if(evt->type == NRF_DRV_SPI_EVENT_DONE)
    {
        spi_tx_done = true;
    }
    else
    {
        // Something is wrong
    }
}



/**
 * @brief SPI initialization.
 * Just the usual way. Nothing special here
 */
uint32_t nrf_drv_mpu_init(void)
{
    
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
    
    return nrf_drv_spi_init(&m_spi_instance, &spi_mpu_config, nrf_drv_mpu_spi_event_handler, NULL);  
}



/**@brief Function to merge a register and a buffer of data
 */
static void merge_register_and_data(uint8_t * new_buffer, uint8_t reg, uint8_t * p_data, uint32_t length)
{
    new_buffer[0] = reg;
    memcpy((new_buffer + 1), p_data, length);
}


/**@brief Function to write a series of bytes. The function merges 
 * the register and the data.
 */
uint32_t nrf_drv_mpu_write_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    
    if(length > MPU_SPI_BUFFER_SIZE - 1) // Must be space for register byte in buffer
    {
        return NRF_ERROR_DATA_SIZE;
    }
    
    uint32_t timeout = MPU_SPI_TIMEOUT;
    // Add write bit to register. 
    reg = reg | MPU_SPI_WRITE_BIT;
    
    merge_register_and_data(spi_tx_buffer, reg, p_data, length + 1);
    
    err_code = nrf_drv_spi_transfer(&m_spi_instance, spi_tx_buffer, length + 1, NULL, 0);
    if(err_code != NRF_SUCCESS) return err_code;


    while((!spi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    spi_tx_done = false;

    return err_code;
}

uint32_t nrf_drv_mpu_write_single_register(uint8_t reg, uint8_t data)
{
    uint32_t err_code;
    uint32_t timeout = MPU_SPI_TIMEOUT;

    uint8_t packet[2] = {reg, data};

    // Add write bit to register. 
    reg = reg | MPU_SPI_WRITE_BIT;
    
    err_code = nrf_drv_spi_transfer(&m_spi_instance, packet, 2, NULL, 0);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!spi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;

    spi_tx_done = false;

    return err_code;
}


uint32_t nrf_drv_mpu_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    uint32_t timeout = MPU_SPI_TIMEOUT;
    
    // Add read bit to register. 
    reg = reg | MPU_SPI_READ_BIT;
	
    // Read data over SPI and store incomming data in spi_rx_buffer
    err_code = nrf_drv_spi_transfer(&m_spi_instance, &reg, 1, spi_rx_buffer, length + 1); // Length + 1 because register byte has to be clocked out before MPU returns data of length 'length'
    if(err_code != NRF_SUCCESS) return err_code;

    while((!spi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    spi_tx_done = false;
	
	// Copy data in spi_rx_buffer over to p_data
	memcpy(p_data, &spi_rx_buffer[1], length);

    return NRF_SUCCESS;
}

#endif // Use SPI drivers

/**
  @}
*/
