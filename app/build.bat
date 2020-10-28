@echo off

adb shell "rm -rf /sdcard/Workshop/*"
pushd src\main\cpp
python build.py
popd
ssh localhost -p 8022 -i C:\Users\DELL\.ssh\id_rsa ./build.sh
adb pull /sdcard/libbridge.so .