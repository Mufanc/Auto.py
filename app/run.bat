@echo off

if exist classes.dex (
    adb push classes.dex /data/local/tmp/
    del classes.dex
    ::adb push .\build\intermediates\cmake\debug\obj\arm64-v8a\libbridge.so /data/local/tmp/
    adb push .\libbridge.so /data/local/tmp/
    ::adb push .\libs\uiautomator.jar /data/local/tmp/
    adb shell "cd /data/local/tmp/ && LD_LIBRARY_PATH=/system/lib64/:/data/local/tmp/:/data/local/tmp/files/usr/lib CLASSPATH=/data/local/tmp/classes.dex:/system/framework/uiautomator.jar app_process /data/local/tmp mfc.autopy.Entrance"
) else (
    del classes.jar
    pushd .\build\intermediates\javac\debug\classes
    jar -cvf ..\..\..\..\..\classes.jar *
    popd
    dx --dex --output=classes.dex classes.jar && run.bat
)
