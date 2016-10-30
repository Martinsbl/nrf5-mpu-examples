 /* 
  * The library is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef DRV_MPU_TWI_H__
#define DRV_MPU_TWI_H__


#include <stdbool.h>
#include <stdint.h>
#include "nrf_drv_twi.h"


#define MPU_TWI_TIMEOUT 		5000 
#define MPU_ADDRESS     		0x68 



/**@brief Event handler used to pass TWI events from main application to the MPU library
 *
 * @param[in]   evt             TWI driver event
 * @retval      uint32_t        Error code
 */
void mpu_twi_event_handler(const nrf_drv_twi_evt_t *evt);
  
  

/**@brief Function to initiate TWI drivers
 *
 * @retval      uint32_t        Error code
 */
uint32_t nrf_drv_mpu_init(void);
	


/**@brief Function for reading an arbitrary register
 *
 * @param[in]   reg             Register to write
 * @param[in]   data            Value
 * @retval      uint32_t        Error code
 */
uint32_t mpu_write_register(uint8_t reg, uint8_t data);

/**@brief Function for reading an arbitrary register
 *
 * @param[in]   reg             Register to write
 * @param[in]   data            Value
 * @param[in]   length          Number of bytes to write
 * @retval      uint32_t        Error code
 */
uint32_t mpu_write_burst(uint8_t reg, uint8_t * p_data, uint32_t length);

/**@brief Function for reading arbitrary register(s)
 *
 * @param[in]   reg             Register to read
 * @param[in]   p_data          Pointer to place to store value(s)
 * @param[in]   length          Number of registers to read
 * @retval      uint32_t        Error code
 */
uint32_t mpu_read_registers(uint8_t reg, uint8_t * p_data, uint32_t length);
    



#endif /* DRV_MPU_TWI_H__ */

/**
  @}
*/

