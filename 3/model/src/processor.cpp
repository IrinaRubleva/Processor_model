#include "../include/processor.hpp"
#include <stdexcept>
#include <iostream>


// processor.cpp
Processor::Processor(uint64_t busAddressSpace, size_t numRegisters)
    : bus(std::make_unique<Bus>(busAddressSpace))
    , controlUnit(std::make_unique<ControlUnit>())
    , running(0)
    , currentTime(0)
{
    std::cout << "=== Processor::Constructor START ===" << std::endl;
    
    // КРИТИЧЕСКО: установить bus в ControlUnit
    controlUnit->setBus(bus.get());
    std::cout << "[Processor] Bus set to ControlUnit" << std::endl;
    
    // Инициализировать регистры - ВОЗМОЖНА ПРОБЛЕМА ЗДЕСЬ!
    std::cout << "[Processor] Initializing registers, count: " << numRegisters << std::endl;
    controlUnit->regs().resize(numRegisters, 0);
    std::cout << "[Processor] Registers initialized" << std::endl;
    
    // Проверка
    std::cout << "[Processor] Bus pointer: " << bus.get() << std::endl;
    std::cout << "[Processor] ControlUnit pointer: " << controlUnit.get() << std::endl;
    std::cout << "[Processor] ControlUnit->getBus(): " << controlUnit->getBus() << std::endl;
    
    std::cout << "=== Processor::Constructor FINISHED ===" << std::endl;
}


// void Processor:: run(){
    
//     try {
//         if (!isValid()) {
//             throw std::runtime_error("Processor not properly initialized");
//         }
//     } catch(const std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }

//     running = true;

//     while (running)
//     {
//         ++currentTime;
//         if(currentTime % controlUnit->getPeriod() == 0){
//             controlUnit->runCycle();
//         }

//         uint64_t gprBase = bus->getDeviceAddress("GPR");
//         uint32_t threadCount = 0;

//         for (int i = 0; i < 4; ++i)
//         {
//             threadCount |= (uint32_t)bus->read(gprBase + i) << (i * 8);
//         }

//         uint64_t pc = controlUnit->getPC();
//         uint64_t progSize = programMemory->getSize();

//         if (threadCount == 0 && pc >= progSize)
//         {
//             std::cout << "[Processor] No active threads and PC out of bounds - stopping\n";
//             running = false;
//             break;
//         }

//         if (currentTime > 1000)
//         {
//             std::cout << "[Processor] Timeout reached - stopping\n";
//             running = false;
//             break;
//         }
//     }
// }



void Processor::run() {
    std::cout << "[Processor::run] START" << std::endl;
    std::cout << "[Processor::run] Bus: " << bus.get() << std::endl;
    std::cout << "[Processor::run] ProgramMemory: " << programMemory.get() << std::endl;
    std::cout << "[Processor::run] ControlUnit: " << controlUnit.get() << std::endl;
    
    running = 1;
    std::cout << "[Processor::run] Processor marked as running" << std::endl;
    
    while (running) {
        std::cout << "[Processor::run] Current time: " << currentTime << std::endl;
        std::cout << "[Processor::run] PC before cycle: " << controlUnit->getPC() << std::endl;
        
        controlUnit->runCycle();
        
        std::cout << "[Processor::run] PC after cycle: " << controlUnit->getPC() << std::endl;
        
        currentTime++;
        
        
        if (currentTime > 10) {  
            running = 0;
            std::cout << "[Processor::run] Time limit reached" << std::endl;
        }
        
        
        if (controlUnit->getPC() >= programMemory->getSize()) {
            running = 0;
            std::cout << "[Processor::run] End of program reached" << std::endl;
        }
    }
    
    running = 0;
    std::cout << "[Processor::run] Processor marked as stopped" << std::endl;
    std::cout << "[Processor::run] FINISHED" << std::endl;
}