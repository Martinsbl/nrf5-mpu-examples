import os
from subprocess import call


project_rootdir = "C:\\Nordic_SDKs\\11.0.0\\examples\\mpu_test\\"
keil_dir = "C:\\Keil_v5\\UV4\\"
logfile_dir = "C:\\Nordic_SDKs\\11.0.0\\examples\\mpu_test\\log_files\\"

test_runs = 0
compile_successes = 0
compile_errors = 0

for subdir, dirs, files in os.walk(project_rootdir):
    for file in files:
        complete_file_path = os.path.join(subdir, file)
        if(complete_file_path.endswith(".uvprojx")):
            if "pca10040" in complete_file_path:
                logfile = file + "_nrf52"
            elif "pca10028" in complete_file_path:
                logfile = file + "_nrf51"
            else:
                logfile = file + "_Unknown device"
            #print("Compiling: " + logfile)
            call([keil_dir + "UV4", "-r", complete_file_path, "-o", logfile_dir + logfile + ".log"])

            # Check for compile errors in log file
            f = open(logfile_dir + logfile + ".log", 'r')
            compile_error = True
            for line in f:
                if "0 Error(s), 0 Warning(s)" in line:
                    compile_error = False
            if compile_error is True:
                print("Failed compiling  : " + logfile)
                compile_errors = compile_errors + 1
            else: 
                print("Success compiling : " + logfile + "!")
                compile_successes = compile_successes + 1

            # Break for testing
            test_runs = test_runs + 1

    if test_runs > 1:
        break
print("\nFINISH!\nNumber of compile errors: " + str(compile_errors) + "\nNumber of compile successes: " + str(compile_successes))