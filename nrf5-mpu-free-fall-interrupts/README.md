# Free fall example for nRF5x and MPU9150
This example will not work on MPU60x0 and MPU9255 as they don't have the same free fall detection hardware as the MPU9150. Nor will it work with SPI interface as MPU9150 only supports the TWI interface. 

# Tested on:
* MPU9150
* nRF51 DK (PCA10028)
* nRF52 DK (PCA10040)
* SDK 11.0.0
* SDK 11.0.0

# How to use:
* All examples are made for SDK 11.0.0. Other SDKs will not work. Download zip file and extract to "sdk_11.0.0_folder\examples"
* Define your MPU9150 in Keil's "Target Options -> C/C++".
* Remember to define the correct TWI pins and interrupt pins. The defines are located in main.c.

# Disclaimer
 * The examples are not extensively tested and only meant as simple explanations and for inspiration. 
 * NO WARRANTY of ANY KIND is provided.
 * There are probably several things that can be improved or added. E.g. the TWI transfer functions should be interrupt driven and not blocking like they are now. 

