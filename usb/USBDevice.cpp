//
// Created by eb1028 on 8/4/2024.
//

#include <sstream>

#include "USBDevice.h"

USBDevice::USBDevice(libusb_device *device) : device(device) {
    retrieveDescriptor();
}

USBDevice::~USBDevice() {}

void USBDevice::retrieveDescriptor() {
    int r = libusb_get_device_descriptor(this->device, &descriptor);
    if(r < 0) {
        throw std::runtime_error("Failed to get device descriptor");
    }
}

std::string USBDevice::getIdentifier() const {
    std::ostringstream oss;
    oss << std::hex << descriptor.idVendor << ":" << descriptor.idProduct;
    return oss.str();
}

std::string USBDevice::getDescriptor() const {
    libusb_device_handle *handle;
    if (libusb_open(device, &handle) != 0) {
        throw std::runtime_error("Failed to open device");
    }

    unsigned char buffer[256];
    std::string manufacturer, product;

    // Get manufacturer string
    if (descriptor.iManufacturer) {
        int ret = libusb_get_string_descriptor_ascii(handle, descriptor.iManufacturer, buffer, sizeof(buffer));
        if (ret > 0) {
            manufacturer = std::string(buffer, buffer + ret);
        }
    }

    // Get product string
    if (descriptor.iProduct) {
        int ret = libusb_get_string_descriptor_ascii(handle, descriptor.iProduct, buffer, sizeof(buffer));
        if (ret > 0) {
            product = std::string(buffer, buffer + ret);
        }
    }

    libusb_close(handle);

    std::ostringstream oss;
    oss << manufacturer << " " << product;
    return oss.str();
}

std::shared_ptr<Session> USBDevice::openSession() const {
    libusb_device_handle *handle;
    if(libusb_open(device, &handle) != 0) {
        throw std::runtime_error("Failed to open device");
    }

    return std::make_shared<USBSession>(handle);
}