# WORK IN PROGRESS
MPU9150 example meant to be used with Nordic's nRF51 3rd revision IC and SDK 10.0.0. 

This example, with its MPU9150 library, will also probably work with the MPU6050, but I have not tested it. Quote from Invensense:

<i>"The MPU-6050 is higher power, lower noise, and larger package versus the MPU-6500. Most of your code should port over, but some low power features are different and will need to be recoded in. Basic data acquisitions shouldn’t have changed.

The MPU-9150 contains the MPU-6050 and an AK8975 magnetometer from AKM. The MPU-9250 contains a MPU-6500 and AK8963. The same differences between gyro/accel are the same you see with 6050 v. 6500. The magnetometer on the MPU-9250 is a little better across the board."</i>

This example and library does not implement the AK8963 or any low power features. Further on, since the MPU6050's and MPU9150's register map is the same it should work fine on both sensors.

More on the difference between MPU6050 and MPU9150: http://www.sureshjoshi.com/embedded/invensense-imus-what-to-know/