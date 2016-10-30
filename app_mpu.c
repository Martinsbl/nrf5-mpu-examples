 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "app_mpu.h"
#include "mpu_register_map.h"
#include "nrf_gpio.h"
#include "nrf_drv_mpu.h"
#include "nrf_error.h"



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



uint32_t mpu_init(void)
{
    uint32_t err_code;
	
	// Initate TWI or SPI driver dependent on what is defined from the project
	err_code = nrf_drv_mpu_init();
    if(err_code != NRF_SUCCESS) return err_code;

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
    uint8_t raw_values[6];
    err_code = mpu_read_registers(MPU_REG_ACCEL_XOUT_H, raw_values, 6);
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



uint32_t mpu_read_gyro(gyro_values_t * gyro_values)
{
    uint32_t err_code;
    uint8_t raw_values[6];
    err_code = mpu_read_registers(MPU_REG_GYRO_XOUT_H, raw_values, 6);
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



uint32_t mpu_read_temp(temp_value_t * temperature)
{
    uint32_t err_code;
    uint8_t raw_values[2];
    err_code = mpu_read_registers(MPU_REG_TEMP_OUT_H, raw_values, 2);
    if(err_code != NRF_SUCCESS) return err_code;

    *temperature = (temp_value_t)(raw_values[0] << 8) + raw_values[1];

    return NRF_SUCCESS;
}



uint32_t mpu_read_int_source(uint8_t * int_source)
{
    return mpu_read_registers(MPU_REG_INT_STATUS, int_source, 1);
}



// Function does not work on MPU60x0 and MPU9255
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
