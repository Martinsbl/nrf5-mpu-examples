# Examples for nRF51 & nRF52 and MPUxxxx
MPU examples meant to be used with Nordic's nRF51 or nRF52 ICs and SDK 11.0.0. 

# Tested on:
* MPU9255
* MPU9150
* nRF51 DK (PCA10028)
* nRF52 DK (PCA10040)
* SDK 11.0.0
* SoftDevice S132 V2.0.0 and S130 V2.0.0.
* <b>note that the examples are not tested with PCA10036 or MPU6xxx series.</b>

# How to use:
* All examples are made for SDK 11.0.0. Other SDKs will not work. Download zip file and extract to "sdk_11.0.0_folder\examples"
* Define your MPU in Keil's "Target Options -> C/C++". Use define "MPU9255", "MPU9150", or "MPU60x0".
* Remember to define the correct TWI pins and interrupt pins. The defines are located in main.c.

# Disclaimer
<b>
 * The examples are not extensively tested and only meant as simple explanations and for inspiration. 
 * NO WARRANTY of ANY KIND is provided.
 * There are probably several things that can be improved or added. E.g. the TWI transfer functions should be interrupt driven and not blocking like they are now. 
 </b>

# MPUs
Quote on differences between different MPUs from Invensense:
<i>"The MPU-6050 is higher power, lower noise, and larger package versus the MPU-6500. Most of your code should port over, but some low power features are different and will need to be recoded in. Basic data acquisitions shouldn’t have changed.

The MPU-9150 contains the MPU-6050 and an AK8975 magnetometer from AKM. The MPU-9250 contains a MPU-6500 and AK8963. The same differences between gyro/accel are the same you see with 6050 v. 6500. The magnetometer on the MPU-9250 is a little better across the board."</i>

This example and library does not implement the AK8963 or any low power features.

More on the difference between MPU6050 and MPU9150: http://www.sureshjoshi.com/embedded/invensense-imus-what-to-know/

