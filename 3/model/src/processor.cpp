#include "../include/processor.hpp"
#include <stdexcept>
#include <iostream>
#include <memory>

Processor::Processor(uint64_t busAddressSpace, size_t numRegisters)
    : bus(std::make_unique<Bus>(busAddressSpace))
    , controlUnit(std::make_unique<ControlUnit>())
    , running(0)
    , currentTime(0)
{
    controlUnit->setBus(bus.get());
    controlUnit->regs().resize(numRegisters, 0);
}


void Processor::run()
{
    running = 1;
    controlUnit->runProgram();
    running = 0;
}