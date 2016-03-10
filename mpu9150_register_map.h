 /* 
  * This code is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * THE LIST OF REGISTERS MIGHT NOT BE ACCURATE
  * NO WARRANTY of ANY KIND is provided. 
  */

#ifndef MPU9150_REG_MAP_H
#define MPU9150_REG_MAP_H

 // NOT ACCURATE AND TRUSTWORTHY 
 // MUST BE UPDATED

#include <stdbool.h>
#include <stdint.h>

#define MPU_REG_SMPLRT_DIV       0x19
#define MPU_REG_CONFIG           0x1A
#define MPU_REG_GYRO_CONFIG      0x1B
#define MPU_REG_ACCEL_CONFIG     0x1C
#define MPU_REG_FF_THR           0x1D
#define MPU_REG_FF_DUR           0x1E
#define MPU_REG_MOT_THR          0x1F
#define MPU_REG_MOT_DUR          0x20
#define MPU_REG_ZMOT_THR         0x21
#define MPU_REG_ZRMOT_DUR        0x22
#define MPU_REG_FIFO_EN          0x23
#define MPU_REG_I2C_MST_CTRL     0x24   
#define MPU_REG_I2C_SLV0_ADDR    0x25
#define MPU_REG_I2C_SLV0_REG     0x26
#define MPU_REG_I2C_SLV0_CTRL    0x27
#define MPU_REG_I2C_SLV1_ADDR    0x28
#define MPU_REG_I2C_SLV1_REG     0x29
#define MPU_REG_I2C_SLV1_CTRL    0x2A
#define MPU_REG_I2C_SLV2_ADDR    0x2B
#define MPU_REG_I2C_SLV2_REG     0x2C
#define MPU_REG_I2C_SLV2_CTRL    0x2D
#define MPU_REG_I2C_SLV3_ADDR    0x2E
#define MPU_REG_I2C_SLV3_REG     0x2F
#define MPU_REG_I2C_SLV3_CTRL    0x30
#define MPU_REG_I2C_SLV4_ADDR    0x31
#define MPU_REG_I2C_SLV4_REG     0x32
#define MPU_REG_I2C_SLV4_DO      0x33
#define MPU_REG_I2C_SLV4_CTRL    0x34
#define MPU_REG_I2C_SLV4_DI      0x35
#define MPU_REG_I2C_MST_STATUS   0x36
#define MPU_REG_INT_PIN_CFG      0x37
#define MPU_REG_INT_ENABLE       0x38
#define MPU_REG_DMP_INT_STATUS   0x39
#define MPU_REG_INT_STATUS       0x3A
#define MPU_REG_ACCEL_XOUT_H     0x3B
#define MPU_REG_ACCEL_XOUT_L     0x3C
#define MPU_REG_ACCEL_YOUT_H     0x3D
#define MPU_REG_ACCEL_YOUT_L     0x3E
#define MPU_REG_ACCEL_ZOUT_H     0x3F
#define MPU_REG_ACCEL_ZOUT_L     0x40
#define MPU_REG_TEMP_OUT_H       0x41
#define MPU_REG_TEMP_OUT_L       0x42
#define MPU_REG_GYRO_XOUT_H      0x43
#define MPU_REG_GYRO_XOUT_L      0x44
#define MPU_REG_GYRO_YOUT_H      0x45
#define MPU_REG_GYRO_YOUT_L      0x46
#define MPU_REG_GYRO_ZOUT_H      0x47
#define MPU_REG_GYRO_ZOUT_L      0x48
#define MPU_REG_EXT_SENS_DATA_00 0x49
#define MPU_REG_EXT_SENS_DATA_01 0x4A
#define MPU_REG_EXT_SENS_DATA_02 0x4B
#define MPU_REG_EXT_SENS_DATA_03 0x4C
#define MPU_REG_EXT_SENS_DATA_04 0x4D
#define MPU_REG_EXT_SENS_DATA_05 0x4E
#define MPU_REG_EXT_SENS_DATA_06 0x4F
#define MPU_REG_EXT_SENS_DATA_07 0x50
#define MPU_REG_EXT_SENS_DATA_08 0x51
#define MPU_REG_EXT_SENS_DATA_09 0x52
#define MPU_REG_EXT_SENS_DATA_10 0x53
#define MPU_REG_EXT_SENS_DATA_11 0x54
#define MPU_REG_EXT_SENS_DATA_12 0x55
#define MPU_REG_EXT_SENS_DATA_13 0x56
#define MPU_REG_EXT_SENS_DATA_14 0x57
#define MPU_REG_EXT_SENS_DATA_15 0x58
#define MPU_REG_EXT_SENS_DATA_16 0x59
#define MPU_REG_EXT_SENS_DATA_17 0x5A
#define MPU_REG_EXT_SENS_DATA_18 0x5B
#define MPU_REG_EXT_SENS_DATA_19 0x5C
#define MPU_REG_EXT_SENS_DATA_20 0x5D
#define MPU_REG_EXT_SENS_DATA_21 0x5E
#define MPU_REG_EXT_SENS_DATA_22 0x5F
#define MPU_REG_EXT_SENS_DATA_23 0x60
#define MPU_REG_MOT_DETECT_STATUS 0x61
#define MPU_REG_I2C_SLV0_DO      0x63
#define MPU_REG_I2C_SLV1_DO      0x64
#define MPU_REG_I2C_SLV2_DO      0x65
#define MPU_REG_I2C_SLV3_DO      0x66
#define MPU_REG_I2C_MST_DELAY_CTRL 0x67
#define MPU_REG_SIGNAL_PATH_RESET  0x68
#define MPU_REG_MOT_DETECT_CTRL   0x69
#define MPU_REG_USER_CTRL        0x6A
#define MPU_REG_PWR_MGMT_1       0x6B
#define MPU_REG_PWR_MGMT_2       0x6C
#define MPU_REG_DMP_BANK         0x6D
#define MPU_REG_DMP_RW_PNT       0x6E
#define MPU_REG_DMP_REG          0x6F
#define MPU_REG_DMP_REG_1        0x70
#define MPU_REG_DMP_REG_2        0x71
#define MPU_REG_FIFO_COUNTH      0x72
#define MPU_REG_FIFO_COUNTL      0x73
#define MPU_REG_FIFO_R_W         0x74
#define MPU_REG_WHO_AM_I_MPU9150 0x75


/**
 *@}
 **/

#endif /* MPU9150_REG_MAP_H */

