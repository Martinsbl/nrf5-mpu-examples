 /*
  * The library is not extensively tested and only
  * meant as a simple explanation and for inspiration.
  * NO WARRANTY of ANY KIND is provided.
  */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "app_mpu.h"
#include "nrf_gpio.h"
#include "nrf_drv_mpu.h"
#include "nrf_error.h"
#include "nrf_peripherals.h"



uint32_t app_mpu_config(app_mpu_config_t * config)
{
    uint8_t *data;
    data = (uint8_t*)config;
    return nrf_drv_mpu_write_registers(MPU_REG_SMPLRT_DIV, data, 4);
}



uint32_t app_mpu_int_cfg_pin(app_mpu_int_pin_cfg_t *cfg)
{
    uint8_t *data;
    data = (uint8_t*)cfg;
    return nrf_drv_mpu_write_single_register(MPU_REG_INT_PIN_CFG, *data);
    
}



uint32_t app_mpu_int_enable(app_mpu_int_enable_t *cfg)
{
    uint8_t *data;
    data = (uint8_t*)cfg;
    return nrf_drv_mpu_write_single_register(MPU_REG_INT_ENABLE, *data);
}



uint32_t app_mpu_init(void)
{
    uint32_t err_code;
	
	// Initate TWI or SPI driver dependent on what is defined from the project
	err_code = nrf_drv_mpu_init();
    if(err_code != NRF_SUCCESS) return err_code;

    uint8_t reset_value = 7; // Resets gyro, accelerometer and temperature sensor signal paths.
    err_code = nrf_drv_mpu_write_single_register(MPU_REG_SIGNAL_PATH_RESET, reset_value);
    if(err_code != NRF_SUCCESS) return err_code;

    // Chose  PLL with X axis gyroscope reference as clock source
    err_code = nrf_drv_mpu_write_single_register(MPU_REG_PWR_MGMT_1, 1);
    if(err_code != NRF_SUCCESS) return err_code;

    return NRF_SUCCESS;
}



uint32_t app_mpu_read_accel(accel_values_t * accel_values)
{
    uint32_t err_code;
    uint8_t raw_values[6];
    err_code = nrf_drv_mpu_read_registers(MPU_REG_ACCEL_XOUT_H, raw_values, 6);
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



uint32_t app_mpu_read_gyro(gyro_values_t * gyro_values)
{
    uint32_t err_code;
    uint8_t raw_values[6];
    err_code = nrf_drv_mpu_read_registers(MPU_REG_GYRO_XOUT_H, raw_values, 6);
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



uint32_t app_mpu_read_temp(temp_value_t * temperature)
{
    uint32_t err_code;
    uint8_t raw_values[2];
    err_code = nrf_drv_mpu_read_registers(MPU_REG_TEMP_OUT_H, raw_values, 2);
    if(err_code != NRF_SUCCESS) return err_code;

    *temperature = (temp_value_t)(raw_values[0] << 8) + raw_values[1];

    return NRF_SUCCESS;
}



uint32_t app_mpu_read_int_source(uint8_t * int_source)
{
    return nrf_drv_mpu_read_registers(MPU_REG_INT_STATUS, int_source, 1);
}


// Function does not work on MPU60x0 and MPU9255
#if defined(MPU9150)
uint32_t app_mpu_config_ff_detection(uint16_t mg, uint8_t duration)
{
    uint32_t err_code;
    uint8_t threshold = (uint8_t)(mg/MPU_MG_PR_LSB_FF_THR);
    if(threshold > 255) return MPU_BAD_PARAMETER;

    err_code = nrf_drv_mpu_write_single_register(MPU_REG_FF_THR, threshold);
    if(err_code != NRF_SUCCESS) return err_code;

    return nrf_drv_mpu_write_single_register(MPU_REG_FF_DUR, duration);
}
#endif // defined(MPU9150)



/*********************************************************************************************************************
 * FUNCTIONS FOR MAGNETOMETER.
 * MPU9150 has an AK8975C and MPU9255 an AK8963 internal magnetometer. Their register maps
 * are similar, but AK8963 has adjustable resoultion (14 and 16 bits) while AK8975C has 13 bit resolution fixed. 
 */

#if (defined(MPU9150) || defined(MPU9255)) && (MPU_USES_TWI) // Magnetometer only works with TWI so check if TWI is enabled

uint32_t app_mpu_magnetometer_init(app_mpu_magn_config_t * p_magnetometer_conf)
{	
	uint32_t err_code;
	
	// Read out MPU configuration register
	app_mpu_int_pin_cfg_t bypass_config;
	err_code = nrf_drv_mpu_read_registers(MPU_REG_INT_PIN_CFG, (uint8_t *)&bypass_config, 1);
	
	// Set I2C bypass enable bit to be able to communicate with magnetometer via I2C
	bypass_config.i2c_bypass_en = 1;
	// Write config value back to MPU config register
	err_code = app_mpu_int_cfg_pin(&bypass_config);
	if (err_code != NRF_SUCCESS) return err_code;
	
	// Write magnetometer config data	
	uint8_t *data;
    data = (uint8_t*)p_magnetometer_conf;	
    return nrf_drv_mpu_write_magnetometer_register(MPU_AK89XX_REG_CNTL, *data);
}

uint32_t app_mpu_read_magnetometer(magn_values_t * p_magnetometer_values, app_mpu_magn_read_status_t * p_read_status)
{
	uint32_t err_code;
	err_code = nrf_drv_mpu_read_magnetometer_registers(MPU_AK89XX_REG_HXL, (uint8_t *)p_magnetometer_values, 6);
	if(err_code != NRF_SUCCESS) return err_code;
        
	/* Quote from datasheet: MPU_AK89XX_REG_ST2 register has a role as data reading end register, also. When any of measurement data register is read
	in continuous measurement mode or external trigger measurement mode, it means data reading start and
	taken as data reading until ST2 register is read. Therefore, when any of measurement data is read, be
	sure to read ST2 register at the end. */
	if(p_read_status == NULL)
	{
		// If p_read_status equals NULL perform dummy read
		uint8_t status_2_reg;
		err_code = nrf_drv_mpu_read_magnetometer_registers(MPU_AK89XX_REG_ST2, &status_2_reg, 1);
	}
	else
	{
		// If p_read_status NOT equals NULL read and return value of MPU_AK89XX_REG_ST2
		err_code = nrf_drv_mpu_read_magnetometer_registers(MPU_AK89XX_REG_ST2, (uint8_t *)p_read_status, 1);
	}
	return err_code;
}

// Test function for development purposes
uint32_t app_mpu_read_magnetometer_test(uint8_t reg, uint8_t * registers, uint8_t len)
{
    return nrf_drv_mpu_read_magnetometer_registers(reg, registers, len);
}

#endif // (defined(MPU9150) || defined(MPU9255)) && (MPU_USES_TWI) 

/**
  @}
*/
