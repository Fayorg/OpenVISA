//
// Created by Elie Baier on 04.08.2024.
//

#include "USBSession.h"

USBSession::USBSession(libusb_device_handle *handle) : handle(handle), interface_number(0) {
    if (!handle) {
        throw std::runtime_error("Invalid USB device handle");
    }
    bTag = 1;
    claimInterface();
}

USBSession::~USBSession() {
    releaseInterface();
    libusb_close(handle);
}

bool USBSession::sendCommand(const std::vector<uint8_t>& command) {
    /*int transferred;
    int res = libusb_bulk_transfer(handle, /* endpoint / 0x02,
                                   const_cast<uint8_t*>(command.data()),
                                   command.size(), &transferred, 0);
    return res == 0 && transferred == command.size();*/

    size_t pos = 0;
    while (pos < command.size()) {
        bTag = nextbTag(bTag);
        size_t remaining = command.size() - pos;
        size_t thisLen = std::min(remaining, static_cast<size_t>(MAX_TRANSFER_SIZE - BULK_OUT_HEADER_SIZE));

        std::vector<uint8_t> header = encodeBulkOutHeader(bTag, static_cast<uint32_t>(thisLen), true);
        std::vector<uint8_t> transferData(header.begin(), header.end());
        transferData.insert(transferData.end(), command.begin() + pos, command.begin() + pos + thisLen);

        // Align data to 4 bytes
        while (transferData.size() % 4 != 0) {
            transferData.push_back(0x00);
        }

        int actual_length;
        int r = libusb_bulk_transfer(handle, 0x02, transferData.data(), static_cast<int>(transferData.size()), &actual_length, 1000);
        if (r != 0) {
            std::cerr << "Error in bulk transfer: " << libusb_error_name(r) << std::endl;
            return pos;
        }

        pos += thisLen;
    }

    return true;
}

std::vector<uint8_t> USBSession::receiveData(size_t length) {
    /*std::vector<uint8_t> data(length);

    unsigned char data_in[5];
    int transferred;
    int res = libusb_interrupt_transfer(handle, 0x86,
                                   data_in, sizeof(data_in), &transferred, 0);
    if (res != 0) {
        throw std::runtime_error("Failed to receive data");
    }
    data.resize(transferred);
    return data;*/

    std::vector<uint8_t> data(length);
    bool termCharEnabled = true;
    bool termChar = true;

    bTag = nextbTag(bTag);
    std::vector<uint8_t> header = encodeMsgInBulkOutHeader(bTag, static_cast<uint32_t>(data.size()), termCharEnabled, termChar);
    int actual_length;
    int r = libusb_bulk_transfer(handle, 0x02, header.data(), static_cast<int>(header.size()), &actual_length, 1000);
    if (r != 0) {
        std::cerr << "Error in bulk transfer: " << libusb_error_name(r) << std::endl;
    }

    size_t pos = 0;
    size_t transfer = 0;
    while (pos < data.size()) {
        std::vector<uint8_t> buffer(MAX_TRANSFER_SIZE);
        int resp;
        if (pos == 0) {
            resp = readRemoveHeader(buffer);
            transfer = static_cast<size_t>(buffer[4] | (buffer[5] << 8) | (buffer[6] << 16) | (buffer[7] << 24));
        } else {
            resp = readKeepHeader(buffer);
        }

        if (resp < 0) {
            return data;
        }

        std::copy(buffer.begin(), buffer.begin() + resp, data.begin() + pos);
        pos += resp;
        if (pos >= transfer) {
            break;
        }
    }
    return data;
}

int USBSession::readRemoveHeader(std::vector<uint8_t>& data) {
    int actual_length;
    int r = libusb_bulk_transfer(handle, USBTMC_BULK_IN_ENDPOINT, data.data(), static_cast<int>(data.size()), &actual_length, 1000);
    if (r != 0) {
        std::cerr << "Error in bulk transfer: " << libusb_error_name(r) << std::endl;
        return -1;
    }

    // Remove the header and return the size of the payload
    int payload_size = actual_length - BULK_IN_HEADER_SIZE;
    std::copy(data.begin() + BULK_IN_HEADER_SIZE, data.begin() + actual_length, data.begin());
    return payload_size;
}

int USBSession::readKeepHeader(std::vector<uint8_t>& data) {
    int actual_length;
    int r = libusb_bulk_transfer(handle, USBTMC_BULK_IN_ENDPOINT, data.data(), static_cast<int>(data.size()), &actual_length, 1000);
    if (r != 0) {
        std::cerr << "Error in bulk transfer: " << libusb_error_name(r) << std::endl;
        return -1;
    }

    return actual_length;
}

void USBSession::claimInterface() {
    // Checking if a kernel driver is already attached and detach it.
    if(libusb_kernel_driver_active(handle, interface_number)) {
        int r = libusb_detach_kernel_driver(handle, interface_number);
        if(r != 0) {
            throw std::runtime_error("Couldn't detach kernel driver");
        }
    }

    int r = libusb_claim_interface(handle, interface_number);
    if (r != 0) {
        throw std::runtime_error("Failed to claim interface");
    }

    /*int rc = libusb_set_interface_alt_setting(handle, interface_number, 0);
    if(rc != 0) {
        throw std::runtime_error("Cannot configure alternate setting");
    }*/
}

void USBSession::releaseInterface() {
    libusb_release_interface(handle, interface_number);
}

uint8_t USBSession::nextbTag(uint8_t currentTag) {
    return (currentTag % 255) + 1;
}

std::vector<uint8_t> USBSession::encodeBulkOutHeader(uint8_t bTag, uint32_t transferSize, bool eom) {
    std::vector<uint8_t> header(BULK_OUT_HEADER_SIZE, 0);
    header[0] = 1;  // MsgID (USB488-1)
    header[1] = bTag;
    header[2] = ~bTag;
    header[3] = 0;  // Reserved
    header[4] = static_cast<uint8_t>(transferSize & 0xFF);
    header[5] = static_cast<uint8_t>((transferSize >> 8) & 0xFF);
    header[6] = static_cast<uint8_t>((transferSize >> 16) & 0xFF);
    header[7] = static_cast<uint8_t>((transferSize >> 24) & 0xFF);
    header[8] = eom ? 1 : 0;  // EOM
    return header;
}

std::vector<uint8_t> USBSession::encodeMsgInBulkOutHeader(uint8_t bTag, uint32_t transferSize, bool termCharEnabled, uint8_t termChar) {
    std::vector<uint8_t> header(BULK_OUT_HEADER_SIZE, 0);
    header[0] = 2;  // MsgID (Initiate Read)
    header[1] = bTag;
    header[2] = ~bTag;
    header[3] = 0;  // Reserved
    header[4] = static_cast<uint8_t>(transferSize & 0xFF);
    header[5] = static_cast<uint8_t>((transferSize >> 8) & 0xFF);
    header[6] = static_cast<uint8_t>((transferSize >> 16) & 0xFF);
    header[7] = static_cast<uint8_t>((transferSize >> 24) & 0xFF);
    header[8] = termCharEnabled ? 1 : 0;  // TermCharEnable
    header[9] = termChar;  // TermChar
    return header;
}