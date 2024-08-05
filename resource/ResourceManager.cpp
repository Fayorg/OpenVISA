//
// Created by eb1028 on 8/4/2024.
//

#include "ResourceManager.h"

ResourceManager::ResourceManager() {
    libusb_init(nullptr);
    // libusb_set_option(nullptr, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
}

ResourceManager::~ResourceManager() {
    libusb_exit(nullptr);
}

ResourceManager &ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

std::vector<std::shared_ptr<Device>> ResourceManager::findDevice() const {
    std::vector<std::shared_ptr<Device>> devices;
    discoverUSBDevices(devices);

    return devices;
}

void ResourceManager::discoverUSBDevices(std::vector<std::shared_ptr<Device>> &devices) const {
    libusb_device **list;
    ssize_t count = libusb_get_device_list(nullptr, &list);
    for(ssize_t i = 0; i < count; i++) {
        devices.push_back(std::make_shared<USBDevice>(list[i]));
    }
    libusb_free_device_list(list, 1);
}