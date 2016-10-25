 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_drv_spi.h"
#include "drv_mpu_spi.h"
#include "mpu_register_map.h"
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


uint32_t mpu_config(mpu_config_t * config)
{
    uint8_t *data;
    data = (uint8_t*)config;
    return mpu_write_burst(MPU_REG_SMPLRT_DIV, data, 4);
}

uint32_t mpu_int_cfg_pin(mpu_int_pin_cfg_t *cfg)
{
    uint8_t *data;
    data = (uint8_t*)cfg;
    return mpu_write_register(MPU_REG_INT_PIN_CFG, *data);
}

uint32_t mpu_int_enable(mpu_int_enable_t *cfg)
{
    uint8_t *data;
    data = (uint8_t*)cfg;
    return mpu_write_register(MPU_REG_INT_ENABLE, *data);
}



uint32_t mpu_init(nrf_drv_spi_t const * const p_instance)
{
    uint32_t err_code;
    m_spi_instance = p_instance;

    uint8_t reset_value = 7; // Resets gyro, accelerometer and temperature sensor signal paths.
    err_code = mpu_write_register(MPU_REG_SIGNAL_PATH_RESET, reset_value);
    if(err_code != NRF_SUCCESS) return err_code;

    // Chose  PLL with X axis gyroscope reference as clock source
    err_code = mpu_write_register(MPU_REG_PWR_MGMT_1, 1);
    if(err_code != NRF_SUCCESS) return err_code;

    return NRF_SUCCESS;
}


uint32_t mpu_read_accel(accel_values_t * accel_values)
{
    uint32_t err_code;
    uint8_t raw_values[7]; // 7 bytes of raw data to make it easier to include the register byte in the SPI transfer. Should find a better way to do this
    err_code = mpu_read_registers(MPU_REG_ACCEL_XOUT_H, raw_values, 7);
    if(err_code != NRF_SUCCESS) return err_code;

    // Reorganize read sensor values and put them into value struct. Use only the 6 MSB values from the raw data.
    uint8_t *data;
    data = (uint8_t*)accel_values;
    for(uint8_t i = 0; i<6; i++) 
    {
        *data = raw_values[6-i];
        data++;
    }
    return NRF_SUCCESS;
}


uint32_t mpu_read_gyro(gyro_values_t * gyro_values)
{
    uint32_t err_code;
    uint8_t raw_values[7]; // 7 bytes of raw data to make it easier to include the register byte in the SPI transfer. Should find a better way to do this
    err_code = mpu_read_registers(MPU_REG_GYRO_XOUT_H, raw_values, 7);
    if(err_code != NRF_SUCCESS) return err_code;

    // Reorganize read sensor values and put them into value struct. Use only the 6 MSB values from the raw data.
    uint8_t *data;
    data = (uint8_t*)gyro_values;
    for(uint8_t i = 0; i<6; i++)
    {
        *data = raw_values[6-i];
        data++;
    }
    return NRF_SUCCESS;
}

uint32_t mpu_read_temp(temp_value_t * temperature)
{
    uint32_t err_code;
    uint8_t raw_values[3];  // 3 bytes of raw data to make it easier to include the register byte in the SPI transfer. Should find a better way to do this
    err_code = mpu_read_registers(MPU_REG_TEMP_OUT_H, raw_values, 3);
    if(err_code != NRF_SUCCESS) return err_code;

    *temperature = (temp_value_t)(raw_values[1] << 8) + raw_values[2]; // Use only the 2 MSB values from the raw data.

    return NRF_SUCCESS;
}

uint32_t mpu_read_int_source(uint8_t * int_source)
{
    return mpu_read_registers(MPU_REG_INT_STATUS, int_source, 1);
}

// Function does not work on MPU60x0 or MPU9255
#if defined(MPU9150)
uint32_t mpu_config_ff_detection(uint16_t mg, uint8_t duration)
{
    uint32_t err_code;
    uint8_t threshold = (uint8_t)(mg/MPU_MG_PR_LSB_FF_THR);
    if(threshold > 255) return MPU_BAD_PARAMETER;

    err_code = mpu_write_register(MPU_REG_FF_THR, threshold);
    if(err_code != NRF_SUCCESS) return err_code;

    return mpu_write_register(MPU_REG_FF_DUR, duration);
}
#endif

/**
  @}
*/
