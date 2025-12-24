#include "../../include/device/memoryDevice.hpp"

#include <iostream>

uint8_t SingleByteMemoryDevice::read(port_size port) const{
    if (port == 8) { 
        return data;
    }
    return 0xFF;
}

void SingleByteMemoryDevice::write(port_size port, uint8_t value) {
    if (port == 8) {
        data = value;
    }
}


uint8_t SingleByteMemoryDevice_ReadOnly::read(port_size port) const{
    if (port == 8) {
        return data;
    }
    return 0xFF;
}

uint8_t MultiByteMemoryDevice::read(port_size port) const {
    if (port < size) { 
        return buffer[port];
    }
    return 0xFF;
}

void MultiByteMemoryDevice::write(port_size port, uint8_t value) {
    if (port < size) {
        buffer[port] = value;
    }
}

uint8_t MultiByteMemoryDevice_ReadOnly::read(port_size port) const {
    if (port < size) { 
        return buffer[port];
    }
    return 0xFF;
}

void RegisterBlockDevice::write(port_size port, uint8_t value) {
    if (port < count) {
        regs[port] = value;
    }
}

uint8_t RegisterBlockDevice_ReadOnly::read(port_size port) const{
    if (port < count) {
        return regs[port];
    }
    return 0xFF;
}


uint8_t RegisterBlockDevice::read(port_size port) const {
    if (port < count) {
        return regs[port];
    }
    return 0xFF;
}