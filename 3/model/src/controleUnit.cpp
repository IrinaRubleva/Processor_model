#include "../include/controleUnit.hpp"
#include <stdexcept>
#include <iostream>
#include "../include/commands/command.hpp"



void ControlUnit::runCycle()
{
    if (!programMemory || !bus)
    {
        throw std::runtime_error("ControlUnit: no programMemory or bus");
    }

    uint64_t progSize = programMemory->getSize();

    if (PC >= progSize)
    {
        uint64_t gprBase = bus->getDeviceAddress("GPR");
        uint32_t threadCount = 0;
        for (int i = 0; i < 4; ++i)
        {
            threadCount |= (uint32_t)bus->read(gprBase + i) << (i * 8);
        }

        if (threadCount == 0)
        {
            return;
        }
        PC = 0;
        return;
    }

    try
    {
        uint64_t pc_ref = PC;
        std::unique_ptr<Command> cmd = decoder.decode(pc_ref, *programMemory);
        PC = pc_ref;

        if (!cmd)
        {
            return;
        }

        Bus *b = getBus();
        if (!b)
            throw std::runtime_error("ControlUnit::runCycle: bus not set");
        cmd->compute(*this);
    }
    catch (const std::exception &e)
    {
        throw;
    }
}
