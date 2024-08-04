//
// Created by eb1028 on 8/4/2024.
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <iostream>

class ResourceManager {

public:
    static ResourceManager& getInstance();

    std::vector<std::shared_ptr<Device>> findDevice() const;

private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    void discoverUSBDevices(std::vector<std::shared_ptr<Device>>& devices) const;

    std::vec
};



#endif //RESOURCEMANAGER_H
