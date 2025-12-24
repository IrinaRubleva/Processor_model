#include "../../include/device/bus.hpp"

uint8_t Bus::read(uint64_t address) const
{
    auto it = attachedDevices.upper_bound(address);
    if (it == attachedDevices.begin())
        throw std::runtime_error("Bus::read invalid address");

    --it;
    auto &map = it->second;
    if (!map.range(address))
        throw std::runtime_error("Bus::read out of range");

    return map.readDev->read(address - map.baseAddress);
}

void Bus::write(uint64_t address, uint8_t value)
{
    auto it = attachedDevices.upper_bound(address);
    if (it == attachedDevices.begin())
        throw std::runtime_error("Bus::write invalid address");

    --it;
    auto &map = it->second;
    if (!map.range(address))
        throw std::runtime_error("Bus::write out of range");

    dynamic_cast<WriteableDevice *>(map.readDev)->write(address - map.baseAddress, value);
}