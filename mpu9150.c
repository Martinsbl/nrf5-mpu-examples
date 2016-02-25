/*
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_drv_twi.h"
#include "mpu9150.h"
#include "mpu9150_register_map.h"
#include "nrf_gpio.h"

static nrf_drv_twi_t const * m_twi_instance;
volatile static bool twi_tx_done = false;
volatile static bool twi_rx_done = false;


void mpu9150_twi_event_handler(const nrf_drv_twi_evt_t *evt)
{ 
    switch(evt->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
            nrf_gpio_pin_toggle(24);
            switch(evt->xfer_desc.type)
            {
                case NRF_DRV_TWI_XFER_TX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXTX:
                    twi_tx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_RX:
                    twi_rx_done = true;
                    break;
                case NRF_DRV_TWI_XFER_TXRX:
                    twi_rx_done = true;
                    break;
                default:
                    break;
            }
            break;
        case NRF_DRV_TWI_EVT_ADDRESS_NACK:
            break;
        case NRF_DRV_TWI_EVT_DATA_NACK:
            break;
        default:
            break;
    }      
}

static void stupid_buffer_merger(uint8_t * new_buffer, uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint8_t *ptr_new_buffer;
    uint8_t *ptr_data_place_holder;
    
    ptr_data_place_holder = p_data;
    ptr_new_buffer = new_buffer;
    *ptr_new_buffer = reg;
    ptr_new_buffer++;
    
    for(int i = 0; i < length; i++)
    {
        *ptr_new_buffer = *ptr_data_place_holder;
        ptr_new_buffer++;
        ptr_data_place_holder++;
    }
}


static uint32_t mpu9150_write_burst(uint8_t reg, uint8_t * p_data, uint32_t length)
{    
    uint32_t err_code;
    uint32_t timeout = MPU9150_TWI_TIMEOUT;
    
    uint8_t buffer[20];
    stupid_buffer_merger(buffer, reg, p_data, length);
    
    nrf_drv_twi_xfer_desc_t xfer_desc;
    xfer_desc.address = MPU9150_ADDRESS;
    xfer_desc.type = NRF_DRV_TWI_XFER_TX;
    xfer_desc.primary_length = length + 1;
    xfer_desc.p_primary_buf = buffer;
    
    err_code = nrf_drv_twi_xfer(m_twi_instance, &xfer_desc, 0);
    
    while((!twi_tx_done) && --timeout);  
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_tx_done = false;
   

//    err_code = nrf_drv_twi_tx(m_twi_instance, MPU9150_ADDRESS, &reg, 1, true);
//    if(err_code != NRF_SUCCESS) return err_code;
//    
//    while((!twi_tx_done) && timeout--);  
//    if(!timeout) return NRF_ERROR_TIMEOUT;
//    twi_tx_done = false;
//    
//    err_code = nrf_drv_twi_tx(m_twi_instance, MPU9150_ADDRESS, p_data, length, false);
//    if(err_code != NRF_SUCCESS) return err_code;
//    
//    timeout = MPU9150_TWI_TIMEOUT;
//    while((!twi_tx_done) && timeout--);
//    if(!timeout) return NRF_ERROR_TIMEOUT;
//    twi_tx_done = false;
    
    return err_code;
}

static uint32_t mpu9150_write_single(uint8_t reg, uint8_t data)
{
    uint32_t err_code;
    uint32_t timeout = MPU9150_TWI_TIMEOUT;
    
    uint8_t packet[2] = {reg, data};
    
    err_code = nrf_drv_twi_tx(m_twi_instance, MPU9150_ADDRESS, packet, 2, false);
    if(err_code != NRF_SUCCESS) return err_code;
    
    while((!twi_tx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    
    twi_tx_done = false;
    
    return err_code;
}


uint32_t mpu9150_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length)
{
    uint32_t err_code;
    uint32_t timeout = MPU9150_TWI_TIMEOUT;
    
    err_code = nrf_drv_twi_tx(m_twi_instance, MPU9150_ADDRESS, &reg, 1, false);
    if(err_code != NRF_SUCCESS) return err_code;
    
    while((!twi_tx_done) && --timeout);  
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_tx_done = false;
    
    err_code = nrf_drv_twi_rx(m_twi_instance, MPU9150_ADDRESS, p_data, length);
    if(err_code != NRF_SUCCESS) return err_code;
    
    timeout = MPU9150_TWI_TIMEOUT;
    while((!twi_rx_done) && --timeout);
    if(!timeout) return NRF_ERROR_TIMEOUT;
    twi_rx_done = false;
    
    return err_code;
}


uint32_t mpu9150_config(mpu9150_config_t * config)
{
    uint8_t *data; 
    data = (uint8_t*)config;
    return mpu9150_write_burst(MPU9150_REG_SMPLRT_DIV, data, 4);
}

uint32_t mpu9150_int_cfg_pin(mpu9150_int_pin_cfg_t *cfg)
{
    uint8_t *data; 
    data = (uint8_t*)cfg;
    return mpu9150_write_single(MPU9150_REG_INT_PIN_CFG, *data);
}

uint32_t mpu9150_int_enable(mpu9150_int_enable_t *cfg)
{
    uint8_t *data; 
    data = (uint8_t*)cfg;
    return mpu9150_write_single(MPU9150_REG_INT_ENABLE, *data);
}
    
    

uint32_t mpu9150_init(nrf_drv_twi_t const * const p_instance)
{
    uint32_t err_code;
    m_twi_instance = p_instance;
    
    uint8_t reset_value = 7; // Resets gyro, accelerometer and temperature sensor signal paths.
    err_code = mpu9150_write_single(MPU9150_REG_SIGNAL_PATH_RESET, reset_value);
    if(err_code != NRF_SUCCESS) return err_code;
    
    // Chose  PLL with X axis gyroscope reference as clock source
    err_code = mpu9150_write_single(MPU9150_REG_PWR_MGMT_1, 1);
    if(err_code != NRF_SUCCESS) return err_code;
    
    return NRF_SUCCESS;
}


uint32_t mpu9150_read_accel(accel_values_t * accel_values)
{
    uint32_t err_code;
    uint8_t raw_values[6];
    err_code = mpu9150_read_registers(MPU9150_REG_ACCEL_XOUT_H, raw_values, 6);
    if(err_code != NRF_SUCCESS) return err_code;
    
    // Reorganize read sensor values and put them into value struct
    uint8_t *data;
    data = (uint8_t*)accel_values;
    for(uint8_t i = 0; i<6; i++)
    {
        *data = raw_values[5-i];
        data++;
    }
    return NRF_SUCCESS;
}


uint32_t mpu9150_read_gyro(gyro_values_t * gyro_values)
{
    uint32_t err_code;
    uint8_t raw_values[6];
    err_code = mpu9150_read_registers(MPU9150_REG_GYRO_XOUT_H, raw_values, 6);
    if(err_code != NRF_SUCCESS) return err_code;
    
    // Reorganize read sensor values and put them into value struct
    uint8_t *data;
    data = (uint8_t*)gyro_values;
    for(uint8_t i = 0; i<6; i++)
    {
        *data = raw_values[5-i];
        data++;
    }
    return NRF_SUCCESS;
}

uint32_t mpu9150_read_temp(temp_value_t * temperature)
{
    uint32_t err_code;
    uint8_t raw_values[2];
    err_code = mpu9150_read_registers(MPU9150_REG_TEMP_OUT_H, raw_values, 2);
    if(err_code != NRF_SUCCESS) return err_code;  
    
    *temperature = (temp_value_t)(raw_values[0] << 8) + raw_values[1];
    
    return NRF_SUCCESS;    
}

uint32_t mpu9150_read_int_source(uint8_t * int_source)
{
    return mpu9150_read_registers(MPU9150_REG_INT_STATUS, int_source, 1);
}

/**
  @}
*/
