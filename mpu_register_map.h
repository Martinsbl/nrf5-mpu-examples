 /* 
  * This code is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef MPU_REG_MAP_H
#define MPU_REG_MAP_H

#if defined(MPU60x0)
    #include "mpu60x0_register_map.h"
#elif defined(MPU9150)
    #include "mpu9150_register_map.h"
	#include "mpu_ak89xx_magnetometer_register_map.h" // MPU9150 Includes AK8979C Magnetometer
#elif defined(MPU9255)
    #include "mpu9255_register_map.h"
	#include "mpu_ak89xx_magnetometer_register_map.h" // MPU9255 Includes AK8963 Magnetometer
#else 
    #error "No MPU defined. Please define MPU in Target Options C/C++ Defines"
#endif

#endif /* MPU9255_REG_MAP_H */

/**
 *@}
 **/
