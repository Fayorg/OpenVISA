cmake_minimum_required(VERSION 3.28)
project(OpenVISA)

set(CMAKE_CXX_STANDARD 17)

add_executable(OpenVISA main.cpp
        device/Device.cpp
        device/Device.h
        device/Device.h
        usb/USBDevice.cpp
        usb/USBDevice.h
        usb/USBDevice.h)
