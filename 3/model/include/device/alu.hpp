#include "../device/device.hpp"
#include <array>


class ALUDevice_Synchron : public Device, public WriteableDevice, public SynchronDevice {
    OperationType opCode;
    std::array<uint8_t, 8> operandA{};
    std::array<uint8_t, 8> operandB{};
    std::array<uint8_t, 8> result{};
    std::array<uint8_t, 8> flags{};
    bool is_queueExists = false;

    uint64_t period;
public:
    ALUDevice_Synchron(uint_fast64_t period): period(period) {}
    uint8_t read(uint16_t port) override;
    void write(uint16_t port, uint8_t value) override;
    void tick() override;
    uint64_t getPeriod() const override {return period;}

private:
    uint64_t unpack(const std::array<uint8_t,8>& a);
    void pack(std::array<uint8_t,8>& dst, uint64_t val);
    std::pair<uint64_t,uint64_t> compute(OperationType op, uint64_t A, uint64_t B);
};




class ALUDevice_Asynchron : public Device, public WriteableDevice, public AsynchronDevice {
    OperationType opCode;
    std::array<uint8_t, 8> operandA;
    std::array<uint8_t, 8> operandB;
    std::array<uint8_t, 8> result;
    std::array<uint8_t, 8> flags;
    bool is_Ready;

public:
    ALUDevice_Asynchron() = default;
    uint8_t read(uint16_t port) override;
    void write(uint16_t port, uint8_t value) override;
    bool ready() const override {return is_Ready;};

private:
    void tryExecute();
    uint64_t unpack(const std::array<uint8_t,8>& a);
    void pack(std::array<uint8_t,8>& dst, uint64_t val);
    std::pair<uint64_t,uint64_t> compute(OperationType op, uint64_t A, uint64_t B);
};