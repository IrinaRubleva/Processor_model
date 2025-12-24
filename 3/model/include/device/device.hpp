#pragma once
#include <cstdint>
#include <memory>

typedef uint64_t port_size;
class ControlUnit ;

struct ReadableDevice:{
    virtual uint8_t read(port_size port) const = 0; 
    virtual ~ReadableDevice()= default;
};

struct WriteableDevice
{
    virtual void write(uint64_t port, uint8_t value) = 0;
    virtual ~WriteableDevice() = default;
};
//new WritableDevice->write(1,1);

struct SynchronDevice
{
    virtual uint64_t getPeriod() const = 0;
    virtual void tick(ControlUnit& cu) = 0;
    virtual ~SynchronDevice() = default;
};

struct AsynchronDevice
{
    virtual bool ready() const = 0;
    virtual void tick(ControlUnit & cu) = 0;
    virtual ~AsynchronDevice() = default;
};
