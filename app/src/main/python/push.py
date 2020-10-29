import os

for file in os.listdir('.'):
    if file.endswith('.py'):
        if file == 'main.py':
            os.system('adb push main.py /data/local/tmp/')
        elif file != 'push.py' and file != 'bridgeutils.py':
            os.system(f'adb push {file} /data/local/tmp/files/modules/')
