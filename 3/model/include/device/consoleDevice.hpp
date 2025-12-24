#include "../device/device.hpp"


class ConsoleDevice :public ReadableDevice, public WriteableDevice, public AsynchronDevice {
public:
    ConsoleDevice();
    uint8_t read(uint64_t port) const override;
    void write(uint64_t port, uint8_t value) override;
    bool ready() const override;
private:
    uint8_t lastInput;
    bool inputReady;
    uint8_t operationCode;
    uint8_t status;       
};


