import os

for subdir, dirs, files in os.walk(rootdir):
    for file in files:
        complete_file_path = os.join(rootdir, file)
        if(file.endwith(".uvprojx")):
            print("UV4 file")