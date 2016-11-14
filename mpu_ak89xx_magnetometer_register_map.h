

 /* AK89xx Register Map
  * From MPU-9250 Reg map Revision: 1.4
  * Release Date: 09/09/2013
  *
  * This code is not extensively tested and only 
  * meant as a simple explanation and for inspiration. 
  * THE LIST OF REGISTERS MIGHT NOT BE ACCURATE
  * NO WARRANTY of ANY KIND is provided. 
  */
  
#ifndef MPU_AK89XX_REG_MAP_H
#define MPU_AK89XX_REG_MAP_H


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


#endif // MPU_AK89XX_REG_MAP_H
