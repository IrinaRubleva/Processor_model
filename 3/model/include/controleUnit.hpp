#pragma once
#include "device/bus.hpp"
#include "programMemory.hpp"
#include "decoder.hpp"
#include "vector/include/Vector.hpp"
#include <optional>
#include <string>
#include <iostream>

class ControlUnit{
private:
    Decoder decoder;
    Bus* bus = nullptr;
    ProgramMemory* programMemory = nullptr;
    uint64_t PC = 0;
    Vector<uint64_t> registers;
    uint64_t period = 1;
    
    //std::map<std::string, uint64_t> label; //ключ - название label, значение - значение PC, где он встречается 

public:
    //  void loadLabel(const std::map<uint64_t, uint64_t> lab){
    //     label = lab;
    // }
    ControlUnit() : period(1) {}
    ControlUnit(Bus* busPtr, ProgramMemory* progMemPtr)
        : bus(busPtr), programMemory(progMemPtr), PC(0), period(1) {}

    void setBus(Bus* b) { bus = b; }
    void setProgramMemory(ProgramMemory* p) { programMemory = p; }
    void setPC(uint64_t pc) { PC = pc; }
    uint64_t getPC() const { return PC; }

    Bus* getBus() const { return bus; }
    uint64_t getPeriod() const {return period;};
    void setPeriod(uint64_t p) {period = p;};
    Decoder* getDecoder() { return &decoder;}

    Decoder& getRefDecoder(){return decoder;}

    Bus& getRefBus() {
        if (!bus) throw std::runtime_error("ControlUnit: bus is nullptr");
        return *bus;
    }

    const Bus& getRefBus() const {
        if (!bus) throw std::runtime_error("ControlUnit: bus is nullptr");
        return *bus;
    }

    uint64_t getDeviceAddress(const std::string& name) const {
        return bus->getDeviceAddress(name);
    }

    // template<typename T>
    // std::optional<T*>* getDevice(const std::string& name) const {
    //     return bus->getDeviceByName(name);
    // }

    void runCycle();
    
    Vector<uint64_t>& regs() { return registers; }
    const Vector<uint64_t>& regs() const { return registers; }
    uint64_t getReg(size_t idx) const { return registers[idx]; }
    void setReg(size_t idx, uint64_t v) { registers[idx] = v; }
    uint8_t read(port_size port) const {return 0;};
    
};