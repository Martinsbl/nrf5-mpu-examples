# WORK IN PROGRESS
MPU examples meant to be used with Nordic's nRF51 or nRF52 ICs and SDK 11. 

These examples, with the MPU library, are tested only on MPU9150 and MPU9255. The register maps for the MPUxxxx series differ slightly, but most of the library should probably work on the entire series (FOR NOW). Quote from Invensense:

<i>"The MPU-6050 is higher power, lower noise, and larger package versus the MPU-6500. Most of your code should port over, but some low power features are different and will need to be recoded in. Basic data acquisitions shouldn’t have changed.

The MPU-9150 contains the MPU-6050 and an AK8975 magnetometer from AKM. The MPU-9250 contains a MPU-6500 and AK8963. The same differences between gyro/accel are the same you see with 6050 v. 6500. The magnetometer on the MPU-9250 is a little better across the board."</i>

This example and library does not implement the AK8963 or any low power features. Further on, since the MPU6050's and MPU9150's register map is the same it should work fine on both sensors.

More on the difference between MPU6050 and MPU9150: http://www.sureshjoshi.com/embedded/invensense-imus-what-to-know/