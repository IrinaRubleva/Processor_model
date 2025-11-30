#pragma once
#include <vector>
#include <memory>

#include "bus.hpp"
#include "controlUnit.hpp"
#include "decoder.hpp"
#include "device/device.hpp"
#include "device/alu.hpp"
#include "generalRegisters.hpp"
#include "programMemory.hpp"


class Processor {
private:
    Bus bus;                          
    ProgramMemory programMemory;                     
    ControlUnit cu;              

    
    std::vector<std::unique_ptr<Device>> devices;
  //  std::vector<std::unique_ptr<GeneralRegister>> registers;

    bool running = false;
    uint64_t currentTime = 0;

public:
    Processor(uint64_t memorySize);

    void attachDevice(uint64_t base, uint64_t size, std::unique_ptr<Device> dev){
        bus.attachDevice(base, size, dev.get());
        devices.push_back(std::move(dev));
    }

    void loadProgram(const std::vector<uint8_t>& bytes){
        
    }

    void run();    
};


void Processor:: run(){
    running = true;
    while(running){
        ++currentTime;
        cu.runCycle();

        // for (auto& d : devices) {
        
        //     if (auto* sync = dynamic_cast<SynchronDevice*>(d.get())) {
        //         if (currentTime % sync->getPeriod() == 0)
        //             sync->tick();
        //     }
        //     if (auto* async = dynamic_cast<AsynchronDevice*>(d.get())) {
        //         if (async->ready())
        //             async->tick();
        //     }
        // }
    }
} 

