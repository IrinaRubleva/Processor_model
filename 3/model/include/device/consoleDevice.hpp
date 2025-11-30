#include "../device/device.hpp"


class ConsoleDevice : public Device, public WriteableDevice, public AsynchronDevice {
public:
    ConsoleDevice();
    uint8_t read(uint16_t port) override;
    void write(uint16_t port, uint8_t value) override;
    bool ready() const override;
private:
    uint8_t lastInput;
    bool inputReady;
};