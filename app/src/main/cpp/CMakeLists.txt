# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(
        # Sets the name of the library.
        bridge

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        bridge.cpp
)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library(
        # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log
)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(
        # Specifies the target library.
        bridge

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
)

#  -------------------------------------------------------------------------------------------------
#link_directories(
#        ${PROJECT_BINARY_DIR}app\\libs\\lib
#)


include_directories(
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Doc\\includes
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Include\\cpython
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Include\\internal
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Include
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Mac\\PythonLauncher
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\cjkcodecs\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\cjkcodecs
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\expat
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_blake2\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_blake2\\impl
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_blake2
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_ctypes\\darwin
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_ctypes\\libffi_osx\\include
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_ctypes\\libffi_osx\\powerpc
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_ctypes
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_decimal\\libmpdec
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_decimal
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_io\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_io
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_multiprocessing\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_multiprocessing
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_sha3\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_sha3\\kcp
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules\\_sqlite
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Modules
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Objects\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Objects\\stringlib\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Objects\\stringlib
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Objects
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Parser
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\PC\\bdist_wininst
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\PC\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\PC
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Python\\clinic
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Python
#        ${PROJECT_SOURCE_DIR}\\include\\Python\\Tools\\msi\\bundle\\bootstrap
        C:\\Users\\DELL\\AppData\\Local\\Programs\\Python\\Python38\\include
        D:\\Users\\DELL\\AppData\\Local\\Android\\Sdk\\ndk\\21.3.6528147\\sources\\third_party\\shaderc\\libshaderc_util\\include\\libshaderc_util
)
