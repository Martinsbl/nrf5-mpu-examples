import os
from shutil import copyfile
import subprocess

working_directory = "C:\\Kode\\Nordic\\SDK_11\\examples\\nrf5-mpu-examples\\"

## Copy project files into spi and twi folders.
# for root, dirs, files in os.walk(working_directory):
    # for file in files:
        # filepath = os.path.join(root, file)
        # if (file.endswith(".uvprojx") or file.endswith(".uvoptx")) and ("nrf5-ble-uart-mpu-data-ready-interrupts" in filepath) : # find project files that is not specific to nrf52

            ## First copy project to SPI folder
            # if not os.path.exists(root + "\\spi\\"):
                # print("Making dir " + root + "\\spi")
                # os.makedirs(root + "\\spi")
            # print("Copying " + file + " to " + "\\spi")
            # copyfile(filepath, root + "\\spi\\" + file)

            ## GIT Move project to TWI folder
            # if not os.path.exists(root + "\\twi\\"):
                # print("Making dir " + root + "\\twi")
                # os.makedirs(root + "\\twi")
            # print("GIT moving " + file + " to " + "\\twi")
            # subprocess.call(["git", "mv", filepath, root + "\\twi\\" + file])



##Clean up old unused project files
# for root, dirs, files in os.walk(working_directory):
    # for file in files:
        # filepath = os.path.join(root, file)
        # if "arm5_no_packs" in filepath and ("twi" not in root) and ("spi" not in root) and ("nrf5-ble-uart-mpu-data-ready-interrupts" in filepath):
            # print("Removing " + filepath)
            # os.remove(filepath)



##Change relative paths in Keil project files
# filedata = None
# for root, dirs, files in os.walk(working_directory):
    # for file in files:
		# filepath = os.path.join(root, file)
		# if (file.endswith(".uvprojx") or file.endswith(".uvoptx")) and ("nrf5-ble-uart-mpu-data-ready-interrupts" in filepath) : # find project files that is not specific to nrf52
			# with open(filepath, 'r') as f :
				# filedata = f.read()

			## Replace the target string
			# filedata = filedata.replace('>..\\..\\', '>..\\..\\..\\')
			# filedata = filedata.replace(';..\\..\\', ';..\\..\\..\\')

			## Write the file out again
			# with open(filepath, 'w') as f:
				# f.write(filedata)
			# print("Changed relative paths in " + file)
			

			
# Change projects in TWI folders to use twi drivers
filedata = None
for root, dirs, files in os.walk(working_directory):
    for file in files:
		filepath = os.path.join(root, file)
		if (file.endswith(".uvprojx") or file.endswith(".uvoptx")) and ("nrf5-ble-uart-mpu-data-ready-interrupts" in filepath) : # find project files that is not specific to nrf52
			if "twi" in filepath:
				with open(filepath, 'r') as f :
					filedata = f.read()

				# Replace the target string
				# filedata = filedata.replace('spi_', 'twi_')
				# filedata = filedata.replace('_spi', '_twi')
				# filedata = filedata.replace('SPI0', 'TWI0')
				filedata = filedata.replace('ble_app_uart_s130_pca10028', 'twi_sensor_pca10028')
				filedata = filedata.replace('ble_app_uart_s132_pca10040', 'twi_sensor_pca10040')

				# Write the file out again
				with open(filepath, 'w') as f:
					f.write(filedata)
				print("Switched TWI and SPI in " + file)
			
			
			
			
			
			
			
			
			
			
			
			
			
			