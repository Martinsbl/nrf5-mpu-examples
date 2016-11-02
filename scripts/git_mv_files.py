import os
from shutil import copyfile
import subprocess

working_directory = "C:\\Kode\\Nordic\\SDK_11\\examples\\nrf5-mpu-examples\\nrf5-ble-uart-mpu-data-ready-interrupts\\"

## Copy project files into spi and twi folders.
for root, dirs, files in os.walk(working_directory):
    for file in files:
        filepath = os.path.join(root, file)
        if file.endswith(".uvprojx") or file.endswith(".uvoptx"):

            # GIT Move project to TWI folder
            if "ble_app_uart_s13" in filepath:
				print("GIT mving file..." + file)
				if file.endswith(".uvprojx") :
					newfilename = "\\nrf5-ble-uart-data-ready-interrupts.uvprojx"
				elif file.endswith(".uvoptx") :
					newfilename = "\\nrf5-ble-uart-data-ready-interrupts.uvoptx"

				subprocess.call(["git", "mv", filepath, root + newfilename])