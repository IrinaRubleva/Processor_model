#pragma once
#include "device.hpp"


#include <iostream>
typedef uint64_t port_size;

class SingleByteMemoryDevice: public ReadableDevice, public WriteableDevice{
public:
    explicit SingleByteMemoryDevice(uint8_t init = 0): data(init){}
    uint8_t read(port_size port) const override;
    void write(port_size port, uint8_t value) override;
private:
    uint8_t data;
};


class SingleByteMemoryDevice_ReadOnly :public ReadableDevice {
public:
    explicit SingleByteMemoryDevice_ReadOnly(uint8_t init = 0): data(init){}
    uint8_t read(port_size port) const override;
private:
    uint8_t data;
};


class MultiByteMemoryDevice : public WriteableDevice, public ReadableDevice {
public:
    MultiByteMemoryDevice(size_t size) 
    : size(size), buffer(std::make_unique<uint8_t[]>(size)) {}
    
    size_t getSize() const {return size;}
    uint8_t read(port_size port) const override;
    void write(port_size port, uint8_t value) override;

private:
    std::unique_ptr<uint8_t[]> buffer;
    size_t size;
};


class MultiByteMemoryDevice_ReadOnly :public ReadableDevice{
public:
    explicit MultiByteMemoryDevice_ReadOnly(size_t size)
    : size(size), buffer(std::make_unique<uint8_t[]>(size)) {}
    
    uint8_t read(port_size port) const override;
private:
    std::unique_ptr<uint8_t[]> buffer;
    size_t size;
};


class RegisterBlockDevice :public WriteableDevice, public ReadableDevice {
public:
    RegisterBlockDevice(size_t count)
    : count(count), regs(std::make_unique<uint8_t[]>(count)) {}

    uint8_t read(port_size port) const override;
    void write(port_size port, uint8_t value) override;
private:
    std::unique_ptr<uint8_t[]> regs;
    size_t count;
};


class RegisterBlockDevice_ReadOnly : public ReadableDevice{
public:
    RegisterBlockDevice_ReadOnly(size_t count)
    : count(count), regs(std::make_unique<uint8_t[]>(count)) {}

    uint8_t read(port_size port) const override;
private:
    std::unique_ptr<uint8_t[]> regs;
    size_t count;
};
