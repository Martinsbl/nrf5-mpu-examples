# Magnetometer example for nRF5x and MPU9255 and MPU9150
This example will not work on MPU60x0 as it does not have a magnetometer. Nor will it work with SPI interface as there is no way of controlling the magnetometer with SPI.


# How to use:
* All examples are made for SDK 11.0.0. Other SDKs will not work. Download zip file and extract to "sdk_11.0.0_folder\examples"
* Define your MPU (MPU9150 or MPU9255) in Keil's "Target Options -> C/C++".
* Remember to define the correct TWI pins and interrupt pins. The defines are located in main.c.

# Disclaimer
 * The examples are not extensively tested and only meant as simple explanations and for inspiration. 
 * NO WARRANTY of ANY KIND is provided.
 * There are probably several things that can be improved or added. E.g. the TWI transfer functions should be interrupt driven and not blocking like they are now. 

