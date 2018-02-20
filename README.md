# Examples for nRF52 series and MPUxxxx
MPU examples meant to be used with Nordic's nRF52832 and nRF52840 ICs and SDK 14.2.0. 
This repository includes MPU hardware drivers for SPI and TWI. The drivers are located in nrf_drv_mpu_spi.c and nrf_drv_mpu_twi.c respectively. A library is running on top of the drivers. The library is located in app_mpu.h/c. By using the library the hardware driver is transparent to the application and it is easy to switch between SPI and TWI. <b>Note that MPU9150 does not support SPI</b>.

<b>!WARNING!</b>
As of SDK 13 the BLE examples no longer compile to a size smaller than the code size limitation of Keil at 32kB with optimization level -O0 and logger funtionallity turned on. This makes it harder to debug the code and without the logger module nothing will get printed to the UART or RTT.

# Tested on:
* MPU9255
* nRF52 DK (PCA10040)
* SDK 14.2.0
* SoftDevice S132 V5.0.0
* <b>Not tested on PCA10056, but code should compile</b>
* <b>Not tested on MPU60x0 or MPU9150</b>

# How to use:
* All examples are made for SDK 14.2.0. Other SDKs will not work. Download the zip file and extract to "sdk_14.2.0_folder\examples". Or just clone it to the same folder. 
* Define your MPU in Keil's "Target Options -> C/C++". Use define "MPU9255", "MPU9150", or "MPU60x0".
* Decide whether to use TWI or SPI in Keil's "Target Options -> C/C++" by using defines MPU_USES_SPI or MPU_USES_TWI.
* Select SPI_ENABLED <b>or</b> TWI_ENABLED in sdk_config.h dependent on your choice above. 
* Remember to define the correct TWI, SPI, and/or interrupt pins. The SPI and TWI pin defines are located in the nrf_drv_mpu_xxi.c files. The GPIOTE interrupt pin is defined in main.c.

# Disclaimer
 * The examples are not extensively tested and only meant as simple explanations and for inspiration. 
 * NO WARRANTY of ANY KIND is provided.
 * There are probably several things that can be improved or added. E.g. the SPI and TWI transfer functions should be interrupt driven and not blocking like they are now. 

# MPUs
Quote on differences between different MPUs from Invensense:
<i>"The MPU-6050 is higher power, lower noise, and larger package versus the MPU-6500. Most of your code should port over, but some low power features are different and will need to be recoded in. Basic data acquisitions shouldn’t have changed.

The MPU-9150 contains the MPU-6050 and an AK8975 magnetometer from AKM. The MPU-9250 contains a MPU-6500 and AK8963. The same differences between gyro/accel are the same you see with 6050 v. 6500. The magnetometer on the MPU-9250 is a little better across the board."</i>

These examples do not implement the AK8963, Digital Motion Processing (DMP), or any low power features.

More on the difference between MPU6050 and MPU9150: http://www.sureshjoshi.com/embedded/invensense-imus-what-to-know/
