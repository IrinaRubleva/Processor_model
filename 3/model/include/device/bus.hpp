#pragma once
#include "device.hpp"
#include <map>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <optional>

struct DeviceMapping
{
    std::string name;
    uint64_t baseAddress;
    uint64_t size;
    ReadableDevice *readDev;

    bool range(uint64_t addr) const
    {
        return addr >= baseAddress && addr < baseAddress + size;
    }
};

class Bus : public WriteableDevice, public ReadableDevice
{
private:
    uint64_t addressSpaceSize;
    std::map<uint64_t, DeviceMapping> attachedDevices; // key = baseAddress

public:
    ~Bus() noexcept override = default;
    explicit Bus(uint64_t size = 0x1000) : addressSpaceSize(size) {}

    uint64_t getAddressSpaceSize() const { return addressSpaceSize; }
    void setAddressSpaceSize(uint64_t base) noexcept { addressSpaceSize = base; }

    void attachDevice(const std::string &name, uint64_t baseAddress, uint64_t size,
                      ReadableDevice *readDev)
    {
        attachedDevices.emplace(baseAddress,
                                DeviceMapping{name, baseAddress, size, readDev});
    }

    uint64_t getDeviceAddress(const std::string &name) const{
        for (const auto &[base, m] : attachedDevices)
            if (m.name == name)
                return m.baseAddress;

        throw std::runtime_error("Bus: unknown device name: " + name);
    }


    uint64_t getDeviceSize(const std::string &name) const{
        for (const auto &[base, m] : attachedDevices){
            if (m.name == name)
                return m.size;
        }
        return 0;
    }


    template<typename T>
    std::optional<T*> getDeviceByName(const std::string& name) const {
        return getDevice<T>(name);
    }

    template <typename T>
    std::optional<T*> getDevice(const std::string& name) const
    {
        for (const auto& [base, mapping] : attachedDevices) {
            if (mapping.name == name) {
                if (auto* device = static_cast<T*>(mapping.readDev)) {
                    return device;
                }
                break;
            }
        }
        return std::nullopt;
    }

    

    uint8_t read(uint64_t address) const override;

    void write(uint64_t address, uint8_t value) override;
};
