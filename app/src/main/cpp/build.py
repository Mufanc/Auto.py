import os

files = os.listdir('.')
for file in files:
    if file.endswith('.cpp') or file.endswith('.h'):
        os.system(f'adb push {file} /sdcard/Workshop/')
