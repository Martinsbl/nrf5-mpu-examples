 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_drv_spi.h"
#include "nrf_drv_mpu_spi.h"
#include "nrf_gpio.h"

static nrf_drv_spi_t const * m_spi_instance;
volatile static bool spi_tx_done = false;

uint8_t spi_tx_buffer[MPU_SPI_BUFFER_SIZE];
uint8_t spi_rx_buffer[MPU_SPI_BUFFER_SIZE];


void mpu_spi_event_handler(const nrf_drv_spi_evt_t *evt)
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

/**@brief Function to merge a register and a buffer of data
 */
static void buffer_merger(uint8_t * new_buffer, uint8_t reg, uint8_t * p_data, uint32_t length)
{
    new_buffer[0] = reg;
    memcpy((new_buffer + 1), p_data, length);
}


/**@brief Function to write a series of bytes. The function merges 
 * the register and the data.
 */
static uint32_t mpu_write_burst(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    
    if(length > MPU_SPI_BUFFER_SIZE - 1) // Must be space for register byte in buffer
    {
        return MPU_ERROR_BUFFER_TOO_SMALL;
    }
    
    uint32_t timeout = MPU_SPI_TIMEOUT;
    uint8_t merged_buffer[MPU_SPI_BUFFER_SIZE] = {0};
    // Add write bit to register. 
    reg = reg | MPU_SPI_WRITE_BIT;
    
    buffer_merger(merged_buffer, reg, p_data, length + 1);
    
    err_code = nrf_drv_spi_transfer(m_spi_instance, merged_buffer, length + 1, NULL, 0);
    if(err_code != NRF_SUCCESS) return err_code;


    while((!spi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    spi_tx_done = false;

    return err_code;
}

uint32_t mpu_write_register(uint8_t reg, uint8_t data)
{
    uint32_t err_code;
    uint32_t timeout = MPU_SPI_TIMEOUT;

    uint8_t packet[2] = {reg, data};

    // Add write bit to register. 
    reg = reg | MPU_SPI_WRITE_BIT;
    
    err_code = nrf_drv_spi_transfer(m_spi_instance, packet, 2, NULL, 0);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!spi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;

    spi_tx_done = false;

    return err_code;
}


uint32_t mpu_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    uint32_t timeout = MPU_SPI_TIMEOUT;
    
    // Add read bit to register. 
    reg = reg | MPU_SPI_READ_BIT;
    
    err_code = nrf_drv_spi_transfer(m_spi_instance, &reg, 1, p_data, length);
    if(err_code != NRF_SUCCESS) return err_code;

    while((!spi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    spi_tx_done = false;

    return NRF_SUCCESS;
}



/**
  @}
*/
