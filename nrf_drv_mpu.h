 /* 
  * The library is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef NRF_DRV_MPU__
#define NRF_DRV_MPU__


#include <stdbool.h>
#include <stdint.h>

 

/**@brief Function to initiate TWI drivers
 *
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_mpu_init(void);
	


/**@brief Function for reading an arbitrary register
 *
 * @param[in]   reg             Register to write
 * @param[in]   data            Value
 * @param[in]   length          Number of bytes to write
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_mpu_write_registers(uint8_t reg, uint8_t * p_data, uint32_t length);



/**@brief Function for reading an arbitrary register
 *
 * @param[in]   reg             Register to write
 * @param[in]   data            Value
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_mpu_write_single_register(uint8_t reg, uint8_t data);



/**@brief Function for reading arbitrary register(s)
 *
 * @param[in]   reg             Register to read
 * @param[in]   p_data          Pointer to place to store value(s)
 * @param[in]   length          Number of registers to read
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_mpu_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length);
    

uint32_t nrf_drv_mpu_read_magnetometer_registers(uint8_t reg, uint8_t * p_data, uint32_t length);
uint32_t nrf_drv_mpu_write_magnetometer_register(uint8_t reg, uint8_t data);

#endif /* NRF_DRV_MPU__ */

/**
  @}
*/

