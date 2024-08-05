//
// Created by Elie Baier on 04.08.2024.
//

#ifndef OPENVISA_USBSESSION_H
#define OPENVISA_USBSESSION_H
#define MAX_TRANSFER_SIZE 512
#define BULK_OUT_HEADER_SIZE 12
#define BULK_IN_HEADER_SIZE 12

#define USBTMC_BULK_OUT_ENDPOINT 0x02
#define USBTMC_BULK_IN_ENDPOINT 0x86

#include "libusb.h"
#include "../resource/Session.h"

class USBSession : public Session {

public:
    USBSession(libusb_device_handle *handle);
    ~USBSession();

    bool sendCommand(const std::vector<uint8_t>& command) override;
    std::vector<uint8_t> receiveData(size_t length) override;

private:
    libusb_device_handle *handle;
    uint8_t endpoint_in;
    uint8_t endpoint_out;
    uint8_t bTag;
    int interface_number;

    void claimInterface();
    void releaseInterface();
    uint8_t nextbTag(uint8_t currentTag);
    std::vector<uint8_t> encodeBulkOutHeader(uint8_t bTag, uint32_t transferSize, bool eom);
    std::vector<uint8_t> encodeMsgInBulkOutHeader(uint8_t bTag, uint32_t transferSize, bool termCharEnabled, uint8_t termChar);
    int readRemoveHeader(std::vector<uint8_t>& data);
    int readKeepHeader(std::vector<uint8_t>& data);
};


#endif //OPENVISA_USBSESSION_H
