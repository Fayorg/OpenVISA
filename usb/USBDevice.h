//
// Created by eb1028 on 8/4/2024.
//

#ifndef USBDEVICE_H
#define USBDEVICE_H

#include "libusb.h"

#include "../device/Device.h"
#include "../usb/USBSession.h"

class USBDevice : public Device {

public:
    USBDevice(libusb_device *device);
    ~USBDevice();

    std::string getIdentifier() const override;
    std::string getDescriptor() const override;
    std::shared_ptr<Session> openSession() const override;

private:
    libusb_device *device;
    libusb_device_descriptor descriptor;

    void retrieveDescriptor();
};



#endif //USBDEVICE_H
