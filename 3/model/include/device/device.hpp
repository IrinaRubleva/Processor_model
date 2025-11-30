#pragma once
#include <cstdint>
#include <memory>
#include "../bus.hpp"
#include "../operationType.hpp"


class Device{
public:
    virtual uint8_t read(uint16_t port) = 0;
    virtual ~Device() = default;

    Device() = default;
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
};

struct WriteableDevice {
    virtual void write(uint16_t port, uint8_t value) = 0;
    virtual ~WriteableDevice() = default;
};


struct SynchronDevice {
    virtual uint64_t getPeriod() const = 0;
    virtual void tick() = 0;
    virtual ~SynchronDevice() = default;
};


struct AsynchronDevice {
    virtual bool ready() const = 0;
    virtual void tick() = 0;
    virtual ~AsynchronDevice() = default;
};
