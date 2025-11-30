#pragma once
#include "device/device.hpp"
#include <map>
#include <cstdint>
#include  <stdexcept>


class DeviceMapping{
public:
    uint64_t baseAddress;
    uint64_t size;
    Device * device;

    DeviceMapping(uint64_t baseAdd, uint64_t s, Device* dev): baseAddress(baseAdd), size(s), device(dev) {} 
    bool range( uint64_t addr){ //попадает ли адрес device в диапазон
        return (addr >= baseAddress) && (addr <= baseAddress + size);
    }
};


class Bus: public Device {
private:
    uint64_t adressSpaceSize;
    std::map<uint64_t, DeviceMapping> attachDevices;
public:
    Bus(uint64_t size): adressSpaceSize(size) {}

    Device* getDevice(uint64_t address);
    void attachDevice(uint64_t baseAdress, uint64_t size, Device * device){
        attachDevices.emplace(baseAdress, DeviceMapping(baseAdress, size, device));
    }
    
    //uint8_t read(uint16_t port) override = delete;

    uint8_t read(uint64_t address){
        auto baseAdd = attachDevices.upper_bound(address);

        if(baseAdd == attachDevices.begin()){
            throw std::invalid_argument("Bus::read - invalid address");
        }

        --baseAdd;
        DeviceMapping& map = baseAdd->second;
        if(!(map.range(address))){
            throw std::invalid_argument("Bus::read - invalid address in DeviceMapping");
        }
        return map.device->read(address - map.baseAddress);
    }
    void write(uint64_t address, uint8_t value);
};


void Bus::write(uint64_t address, uint8_t value) {
    if (attachDevices.empty()) throw std::out_of_range("Bus::write - no devices");
    auto it = attachDevices.upper_bound(address);
    if (it == attachDevices.begin()) throw std::out_of_range("Bus::write - address below range");
    --it;
    DeviceMapping &map = it->second;
    if (!map.range(address)) throw std::out_of_range("Bus::write - address not mapped");
    uint64_t local = address - map.baseAddress;

    if (auto w = dynamic_cast<WriteableDevice*>(map.device)) {
        w->write(static_cast<uint16_t>(local), value);
    } else {
        throw std::logic_error("Bus::write - device not writeable");
    }
}



Device* Bus::getDevice(uint64_t address) {
    if (attachDevices.empty()) return nullptr;
    auto it = attachDevices.upper_bound(address);
    if (it == attachDevices.begin()) return nullptr;
    --it;
    DeviceMapping &m = it->second;
    if (!m.range(address)) return nullptr;
    return m.device;
}


