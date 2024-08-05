//
// Created by Elie Baier on 04.08.2024.
//

#ifndef OPENVISA_SESSION_H
#define OPENVISA_SESSION_H

#include <iostream>

class Session {

public:
    virtual ~Session() = default;
    virtual bool sendCommand(const std::vector<uint8_t>& command) = 0;
    virtual std::vector<uint8_t> receiveData(size_t length) = 0;
};


#endif //OPENVISA_SESSION_H
