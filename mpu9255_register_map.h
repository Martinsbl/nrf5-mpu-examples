

 /* MPU-9255 Register Map
  * Revision: 1.0
  * Release Date: 10/10/2014
  *
  * This code is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * THE LIST OF REGISTERS MIGHT NOT BE ACCURATE
  * NO WARRANTY of ANY KIND is provided. 
  */
  
#ifndef MPU9255_REG_MAP_H
#define MPU9255_REG_MAP_H


#define MPU_REG_ELF_TEST_X_GYRO    0x00 // Dec 0,   R/W,  xg_st_data [7:0]
#define MPU_REG_ELF_TEST_Y_GYRO    0x01 // Dec 1,   R/W,  yg_st_data [7:0]
#define MPU_REG_ELF_TEST_Z_GYRO    0x02 // Dec 2,   R/W,  zg_st_data [7:0]

#define MPU_REG_SELF_TEST_X_ACCEL  0x0D // Dec 13,  R/W,  XA_ST_DATA [7:0]
#define MPU_REG_SELF_TEST_Y_ACCEL  0x0E // Dec 14,  R/W,  YA_ST_DATA [7:0]
#define MPU_REG_SELF_TEST_Z_ACCEL  0x0F // Dec 15,  R/W,  ZA_ST_DATA [7:0]

#define MPU_REG_XG_OFFSET_H        0x13 // Dec 19,  R/W,  X_OFFS_USR [15:8]
#define MPU_REG_XG_OFFSET_L        0x14 // Dec 20,  R/W,  X_OFFS_USR [7:0]
#define MPU_REG_YG_OFFSET_H        0x15 // Dec 21,  R/W,  Y_OFFS_USR [15:8]
#define MPU_REG_YG_OFFSET_L        0x16 // Dec 22,  R/W,  Y_OFFS_USR [7:0]
#define MPU_REG_ZG_OFFSET_H        0x17 // Dec 23,  R/W,  Z_OFFS_USR [15:8]
#define MPU_REG_ZG_OFFSET_L        0x18 // Dec 24,  R/W,  Z_OFFS_USR [7:0]
#define MPU_REG_SMPLRT_DIV         0x19 // Dec 25,  R/W,  SMPLRT_DIV[7:0]
#define MPU_REG_CONFIG             0x1A // Dec 26,  R/W,  - FIFO_MODE EXT_SYNC_SET[2:0] DLPF_CFG[2:0]
#define MPU_REG_GYRO_CONFIG        0x1B // Dec 27,  R/W,  XGYRO_CtenYGYRO_CtenZGYRO_CtenGYRO_FS_SEL [1:0] - FCHOICE_B[1:0]
#define MPU_REG_ACCEL_CONFIG       0x1C // Dec 28,  R/W,  ax_st_en ay_st_en az_st_en ACCEL_FS_SEL[1:0] -
#define MPU_REG_ACCEL_CONFIG_2     0x1D // Dec 29,  2,    R/W - ACCEL_FCHOICE_B A_DLPF_CFG
#define MPU_REG_LP_ACCEL_ODR       0x1E // Dec 30,  R/W,  - Lposc_clksel [3:0]
#define MPU_REG_WOM_THR            0x1F // Dec 31,  R/W,  WOM_Threshold [7:0]

#define MPU_REG_FIFO_EN            0x23 // Dec 35,  R/W,  TEMP_FIFO_ENGYRO_XOUTGYRO_YOUTGYRO_ZOUT ACCEL SLV2 SLV1 SLV0
#define MPU_REG_I2C_MST_CTRL       0x24 // Dec 36,  R/W,  MULT_MST_ENWAIT_FOR_ESSLV_3_FIFO_ENI2C_MST_P_NSR I2C_MST_CLK[3:0]
#define MPU_REG_I2C_SLV0_ADDR      0x25 // Dec 37,  R/W,  I2C_SLV0_RNW I2C_ID_0 [6:0]
#define MPU_REG_I2C_SLV0_REG       0x26 // Dec 38,  R/W,  I2C_SLV0_REG[7:0]
#define MPU_REG_I2C_SLV0_CTRL      0x27 // Dec 39,  R/W,  I2C_SLV0_ENI2C_SLV0_BYTE_SWI2C_SLV0_REG_DISI2C_SLV0_GRP I2C_SLV0_LENG[3:0]
#define MPU_REG_I2C_SLV1_ADDR      0x28 // Dec 40,  R/W,  I2C_SLV1_RNW I2C_ID_1 [6:0]
#define MPU_REG_I2C_SLV1_REG       0x29 // Dec 41,  R/W,  I2C_SLV1_REG[7:0]
#define MPU_REG_I2C_SLV1_CTRL      0x2A // Dec 42,  R/W,  I2C_SLV1_ENI2C_SLV1_BYTE_SWI2C_SLV1_REG_DISI2C_SLV1_GRP I2C_SLV1_LENG[3:0]
#define MPU_REG_I2C_SLV2_ADDR      0x2B // Dec 43,  R/W,  I2C_SLV2_RNW I2C_ID_2 [6:0]
#define MPU_REG_I2C_SLV2_REG       0x2C // Dec 44,  R/W,  I2C_SLV2_REG[7:0]
#define MPU_REG_I2C_SLV2_CTRL      0x2D // Dec 45,  R/W,  I2C_SLV2_ENI2C_SLV2_BYTE_SWI2C_SLV2_REG_DISI2C_SLV2_GRP I2C_SLV2_LENG[3:0]
#define MPU_REG_I2C_SLV3_ADDR      0x2E // Dec 46,  R/W,  I2C_SLV3_RNW I2C_ID_3 [6:0]
#define MPU_REG_I2C_SLV3_REG       0x2F // Dec 47,  R/W,  I2C_SLV3_REG[7:0]
#define MPU_REG_I2C_SLV3_CTRL      0x30 // Dec 48,  R/W,  I2C_SLV3_ENI2C_SLV3_BYTE_SWI2C_SLV3_REG_DISI2C_SLV3_GRP I2C_SLV3_LENG [3:0]
#define MPU_REG_I2C_SLV4_ADDR      0x31 // Dec 49,  R/W,  I2C_SLV4_RNW I2C_ID_4 [6:0]
#define MPU_REG_I2C_SLV4_REG       0x32 // Dec 50,  R/W,  I2C_SLV4_REG[7:0]
#define MPU_REG_I2C_SLV4_DO        0x33 // Dec 51,  R/W,  I2C_SLV4_DO[7:0]
#define MPU_REG_I2C_SLV4_CTRL      0x34 // Dec 52,  R/W,  I2C_SLV4_ENSLV4_DONE_INT_ENI2C_SLV4_REG_DIS I2C_MST_DLY[4:0]
#define MPU_REG_I2C_SLV4_DI        0x35 // Dec 53,  R,    I2C_SLV4_DI[7:0]
#define MPU_REG_I2C_MST_STATUS     0x36 // Dec 54,  R,    PASS_THROUGHI2C_SLV4_DONEI2C_LOST_ARBI2C_SLV4_NACKI2C_SLV3_NACKI2C_SLV2_NACKI2C_SLV1_NACKI2C_SLV0_NACK
#define MPU_REG_INT_PIN_CFG        0x37 // Dec 55,  R/W,  ACTL OPEN LATCH_INT_ENINT_ANYRD_2CLEARACTL_FSYNCFSYNC_INT_MODE_ENBYPASS_EN -
#define MPU_REG_INT_ENABLE         0x38 // Dec 56,  R/W,  - WOM_EN -FIFO_OFLOW_ENFSYNC_INT_EN - -RAW_RDY_EN

#define MPU_REG_INT_STATUS         0x3A // Dec 58,  R,    - WOM_INT -FIFO_OFLOW_INTFSYNC_INT - -RAW_DATA_RDY_INT
#define MPU_REG_ACCEL_XOUT_H       0x3B // Dec 59,  R,    ACCEL_XOUT_H[15:8]
#define MPU_REG_ACCEL_XOUT_L       0x3C // Dec 60,  R,    ACCEL_XOUT_L[7:0]
#define MPU_REG_ACCEL_YOUT_H       0x3D // Dec 61,  R,    ACCEL_YOUT_H[15:8]
#define MPU_REG_ACCEL_YOUT_L       0x3E // Dec 62,  R,    ACCEL_YOUT_L[7:0]
#define MPU_REG_ACCEL_ZOUT_H       0x3F // Dec 63,  R,    ACCEL_ZOUT_H[15:8]
#define MPU_REG_ACCEL_ZOUT_L       0x40 // Dec 64,  R,    ACCEL_ZOUT_L[7:0]
#define MPU_REG_TEMP_OUT_H         0x41 // Dec 65,  R,    TEMP_OUT_H[15:8]
#define MPU_REG_TEMP_OUT_L         0x42 // Dec 66,  R,    TEMP_OUT_L[7:0]
#define MPU_REG_GYRO_XOUT_H        0x43 // Dec 67,  R,    GYRO_XOUT_H[15:8]
#define MPU_REG_GYRO_XOUT_L        0x44 // Dec 68,  R,    GYRO_XOUT_L[7:0]
#define MPU_REG_GYRO_YOUT_H        0x45 // Dec 69,  R,    GYRO_YOUT_H[15:8]
#define MPU_REG_GYRO_YOUT_L        0x46 // Dec 70,  R,    GYRO_YOUT_L[7:0]
#define MPU_REG_GYRO_ZOUT_H        0x47 // Dec 71,  R,    GYRO_ZOUT_H[15:8]
#define MPU_REG_GYRO_ZOUT_L        0x48 // Dec 72,  R,    GYRO_ZOUT_L[7:0]
#define MPU_REG_EXT_SENS_DATA_00   0x49 // Dec 73,  R,    EXT_SENS_DATA_00[7:0]
#define MPU_REG_EXT_SENS_DATA_01   0x4A // Dec 74,  R,    EXT_SENS_DATA_01[7:0]
#define MPU_REG_EXT_SENS_DATA_02   0x4B // Dec 75,  R,    EXT_SENS_DATA_02[7:0]
#define MPU_REG_EXT_SENS_DATA_03   0x4C // Dec 76,  R,    EXT_SENS_DATA_03[7:0]
#define MPU_REG_EXT_SENS_DATA_04   0x4D // Dec 77,  R,    EXT_SENS_DATA_04[7:0]
#define MPU_REG_EXT_SENS_DATA_05   0x4E // Dec 78,  R,    EXT_SENS_DATA_05[7:0]
#define MPU_REG_EXT_SENS_DATA_06   0x4F // Dec 79,  R,    EXT_SENS_DATA_06[7:0]
#define MPU_REG_EXT_SENS_DATA_07   0x50 // Dec 80,  R,    EXT_SENS_DATA_07[7:0]
#define MPU_REG_EXT_SENS_DATA_08   0x51 // Dec 81,  R,    EXT_SENS_DATA_08[7:0]
#define MPU_REG_EXT_SENS_DATA_09   0x52 // Dec 82,  R,    EXT_SENS_DATA_09[7:0]
#define MPU_REG_EXT_SENS_DATA_10   0x53 // Dec 83,  R,    EXT_SENS_DATA_10[7:0]
#define MPU_REG_EXT_SENS_DATA_11   0x54 // Dec 84,  R,    EXT_SENS_DATA_11[7:0]
#define MPU_REG_EXT_SENS_DATA_12   0x55 // Dec 85,  R,    EXT_SENS_DATA_12[7:0]
#define MPU_REG_EXT_SENS_DATA_13   0x56 // Dec 86,  R,    EXT_SENS_DATA_13[7:0]
#define MPU_REG_EXT_SENS_DATA_14   0x57 // Dec 87,  R,    EXT_SENS_DATA_14[7:0]
#define MPU_REG_EXT_SENS_DATA_15   0x58 // Dec 88,  R,    EXT_SENS_DATA_15[7:0]
#define MPU_REG_EXT_SENS_DATA_16   0x59 // Dec 89,  R,    EXT_SENS_DATA_16[7:0]
#define MPU_REG_EXT_SENS_DATA_17   0x5A // Dec 90,  R,    EXT_SENS_DATA_17[7:0]
#define MPU_REG_EXT_SENS_DATA_18   0x5B // Dec 91,  R,    EXT_SENS_DATA_18[7:0]
#define MPU_REG_EXT_SENS_DATA_19   0x5C // Dec 92,  R,    EXT_SENS_DATA_19[7:0]
#define MPU_REG_EXT_SENS_DATA_20   0x5D // Dec 93,  R,    EXT_SENS_DATA_20[7:0]
#define MPU_REG_EXT_SENS_DATA_21   0x5E // Dec 94,  R,    EXT_SENS_DATA_21[7:0]
#define MPU_REG_EXT_SENS_DATA_22   0x5F // Dec 95,  R,    EXT_SENS_DATA_22[7:0]
#define MPU_REG_EXT_SENS_DATA_23   0x60 // Dec 96,  R,    EXT_SENS_DATA_23[7:0]

#define MPU_REG_I2C_SLV0_DO        0x63 // Dec 99,  R/W,  I2C_SLV0_DO[7:0]
#define MPU_REG_I2C_SLV1_DO        0x64 // Dec 100, R/W,  I2C_SLV1_DO[7:0]
#define MPU_REG_I2C_SLV2_DO        0x65 // Dec 101, R/W,  I2C_SLV2_DO[7:0]
#define MPU_REG_I2C_SLV3_DO        0x66 // Dec 102, R/W,  I2C_SLV3_DO[7:0]
#define MPU_REG_I2C_MST_DELAY_CTRL 0x67 // Dec 103, R/W,  DELAY_ES_SHADOW - -I2C_SLV4_DLY_ENI2C_SLV3_DLY_ENI2C_SLV2_DLY_ENI2C_SLV1_DLY_ENI2C_SLV0_DLY_EN
#define MPU_REG_SIGNAL_PATH_RESET  0x68 // Dec 104, R/W,  - - - - - GYRO_RSTACCEL_RSTTEMP_RST
#define MPU_REG_MOT_DETECT_CTRL    0x69 // Dec 105, R/W,  ACCEL_INTEL_ENACCEL_INTEL_MODE - - -
#define MPU_REG_USER_CTRL          0x6A // Dec 106, R/W,  - FIFO_EN I2C_MST_ENI2C_IF_DIS -FIFO_RSTI2C_MST_RSTSIG_COND_RST
#define MPU_REG_PWR_MGMT_1         0x6B // Dec 107, R/W,  H_RESET SLEEP CYCLE GYRO_STANDBY PD_PTAT CLKSEL[2:0]
#define MPU_REG_PWR_MGMT_2         0x6C // Dec 108, R/W,  - DIS_XA DIS_YA DIS_ZA DIS_XG DIS_YG DIS_ZG

#define MPU_REG_FIFO_COUNTH        0x72 // Dec 114, R/W,  - FIFO_CNT[12:8]
#define MPU_REG_FIFO_COUNTL        0x73 // Dec 115, R/W,  FIFO_CNT[7:0]
#define MPU_REG_FIFO_R_W           0x74 // Dec 116, R/W,  D[7:0]
#define MPU_REG_WHO_AM_I           0x75 // Dec 117, R,    WHOAMI[7:0]

#define MPU_REG_XA_OFFSET_H        0x77 // Dec 119, R/W,  XA_OFFS [14:7]
#define MPU_REG_XA_OFFSET_L        0x78 // Dec 120, R/W,  XA_OFFS [6:0] -

#define MPU_REG_YA_OFFSET_H        0x7A // Dec 122, R/W,  YA_OFFS [14:7]
#define MPU_REG_YA_OFFSET_L        0x7B // Dec 123, R/W,  YA_OFFS [6:0] -

#define MPU_REG_ZA_OFFSET_H        0x7D // Dec 125, R/W,  ZA_OFFS [14:7]
#define MPU_REG_ZA_OFFSET_L        0x7E // Dec 126, R/W,  ZA_OFFS [6:0] -




/* *****************************************************************************
 * MAGNETOMETER REGISTERS
 *******************************************************************************/


#define MPU_AK89XX_REG_WIA	 	0x00 // READ Device ID 8
#define MPU_AK89XX_REG_INFO 	0x01 // READ Information 8
#define MPU_AK89XX_REG_ST1 		0x02 // READ Status 1 Data status
#define MPU_AK89XX_REG_HXL 		0x03 // READ Measurement data X-axis data
#define MPU_AK89XX_REG_HXH 		0x04 //
#define MPU_AK89XX_REG_HYL 		0x05 //
#define MPU_AK89XX_REG_HYH 		0x06 //
#define MPU_AK89XX_REG_HZL 		0x07 //
#define MPU_AK89XX_REG_HZH 		0x08 //
#define MPU_AK89XX_REG_ST2 		0x09 // READ Status 2 Data status
#define MPU_AK89XX_REG_CNTL 	0x0A // READ/WRITE Control 
#define MPU_AK89XX_REG_RST 		0x0B // READ/WRITE Reset DO NOT ACCESS
#define MPU_AK89XX_REG_ASTC 	0x0C // READ/WRITE Self-test 8
#define MPU_AK89XX_REG_TS1 		0x0D // READ/WRITE Test 1 DO NOT ACCESS
#define MPU_AK89XX_REG_TS2 		0x0E // READ/WRITE Test 2 DO NOT ACCESS
#define MPU_AK89XX_REG_I2CDIS 	0x0F // READ/WRITE I2C disable 
#define MPU_AK89XX_REG_ASAX 	0x10 // READ X-axis sensitivity adjustment value Fuse ROM
#define MPU_AK89XX_REG_ASAY 	0x11 // READ Y-axis sensitivity adjustment value Fuse ROM
#define MPU_AK89XX_REG_ASAZ 	0x12 // READ Z-axis sensitivity adjustment value Fuse ROM


#endif // MPU9255_REG_MAP_H
