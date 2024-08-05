//
// Created by eb1028 on 8/4/2024.
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <iostream>
#include <libusb.h>

#include "../device/Device.h"
#include "../usb/USBDevice.h"

class ResourceManager {

public:
    static ResourceManager& getInstance();

    std::vector<std::shared_ptr<Device>> findDevice() const;

private:
    ResourceManager();
    ~ResourceManager();

    void discoverUSBDevices(std::vector<std::shared_ptr<Device>>& devices) const;
};



#endif //RESOURCEMANAGER_H
