#include <iostream>

#include "resource/ResourceManager.h"

std::vector<uint8_t> stringToUint8Vector(const std::string& str) {
    std::vector<uint8_t> vec(str.begin(), str.end());
    return vec;
}

int main()
{
    ResourceManager *rm = &ResourceManager::getInstance();

    auto devices = rm->findDevice();

    for(auto device : devices) {
        std::cout << device->getDescriptor() << std::endl;
    }

    auto rigol = devices[0];
    std::cout << "Selected: " << devices[0]->getDescriptor() << std::endl;

    auto session = rigol->openSession();

    if(session->sendCommand(stringToUint8Vector(std::string("*IDN?")))) {
        std::cout << "Sent success" << std::endl;

        std::vector<uint8_t> response = session->receiveData(50); // Adjust length as needed
        std::cout << "Received data: ";
        for (auto byte : response) {
            std::cout << std::hex << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "Couldn't send" << std::endl;
    }
}

