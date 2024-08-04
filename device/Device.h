//
// Created by eb1028 on 8/4/2024.
//

#ifndef DEVICE_H
#define DEVICE_H

#include <iostream>

class Device {

public:
    virtual ~Device() = default;
    virtual std::string getIdentifier() const = 0;
    virtual std::string getDescriptor() const = 0;
};

#endif //DEVICE_H
