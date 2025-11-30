#include "../device/device.hpp"

class SingleByteMemoryDevice : public Device, public WriteableDevice {
public:
    explicit SingleByteMemoryDevice(uint8_t init = 0);
    uint8_t read(uint16_t port) override;  
    void write(uint16_t port, uint8_t value) override;
private:
    uint8_t data;
};


class SingleByteMemoryDevice_ReadOnly : public Device {
public:
    explicit SingleByteMemoryDevice_ReadOnly(uint8_t init = 0);
    uint8_t read(uint16_t port) override;
private:
    uint8_t data;
};


class MultiByteMemoryDevice : public Device, public WriteableDevice {
public:
    explicit MultiByteMemoryDevice(size_t size);
    uint8_t read(uint16_t port) override;
    void write(uint16_t port, uint8_t value) override;
private:
    std::unique_ptr<uint8_t[]> buffer;
    size_t size;
};


class MultiByteMemoryDevice_ReadOnly : public Device {
public:
    explicit MultiByteMemoryDevice_ReadOnly(size_t size);
    uint8_t read(uint16_t port) override;
private:
    std::unique_ptr<uint8_t[]> buffer;
    size_t size;
};


class RegisterBlockDevice : public Device, public WriteableDevice {
public:
    RegisterBlockDevice(size_t count);
    uint8_t read(uint16_t port) override;
    void write(uint16_t port, uint8_t value) override;
private:
    std::unique_ptr<uint8_t[]> regs;
    size_t count;
};


class RegisterBlockDevice_ReadOnly : public Device {
public:
    RegisterBlockDevice_ReadOnly(size_t count);
    uint8_t read(uint16_t port) override;
private:
    std::unique_ptr<uint8_t[]> regs;
    size_t count;
};
