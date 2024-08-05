cmake_minimum_required(VERSION 3.26)
project(OpenVISA)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

include_directories(${LIBUSB_INCLUDE_DIRS})
link_directories(${LIBUSB_LIBRARY_DIRS})

add_executable(OpenVISA main.cpp
        device/Device.h
        device/Device.h
        usb/USBDevice.cpp
        usb/USBDevice.h
        usb/USBDevice.h
        resource/Session.h
        usb/USBSession.h
        resource/ResourceManager.h
        resource/ResourceManager.cpp
        usb/USBSession.cpp)

target_link_libraries(OpenVISA ${LIBUSB_LIBRARIES})