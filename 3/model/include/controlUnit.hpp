#pragma once
#include "./bus.hpp"
#include "decoder.hpp"
#include "programMemory.hpp"
#include "device/device.hpp"
#include <map>
#include <string>

class ControlUnit: public Device{
private:
    Decoder decoder; 

    Bus* bus; 
    ProgramMemory* programMemory;
    uint64_t PC;
    std::map<std::string, uint64_t> knownDevices; //адреса устройств  - заданы константой
public:
    void setPC(uint64_t pc) noexcept {PC = pc;};
    void runCycle(); //fetch->decode->execute
    uint64_t getDeviceAddress(const std::string& name) const;
    uint64_t read(uint8_t port) override ;
    void write(uint8_t port, uint8_t value) override ;
};


uint64_t ControlUnit::getDeviceAddress(const std::string& name) const {
    auto it = knownDevices.find(name);
    if (it == knownDevices.end()) throw std::runtime_error("unknown device: " + name);
    return it->second;
}



void ControlUnit::runCycle(){
    if (!decoder || !programMemory || !bus) {
        throw std::runtime_error("ControlUnit not fully connected");
    }

    try {
        uint64_t pc = PC;
        std::unique_ptr<Command> cmd = decoder->decode(pc, *programMemory);
        PC = pc;
        if (!cmd) {
            // NOP/end
            return;
        }
        cmd.execute(*bus, *this);
    } catch (const std::exception &e) {
        throw std::runtime_error("ControlUnit::tick error: ");
    }
} 