#pragma once
#include <cstdint>
#include <memory>
#include <vector>

#include "device/bus.hpp"
#include "device/alu.hpp"
#include "device/memoryDevice.hpp"
#include "programMemory.hpp"
#include "controleUnit.hpp"
#include "decoder.hpp"


#include "../include/commands/command.hpp"  
#include "../include/commands/registerCommand.hpp" 

void registerAllCommands(CommandFactory& registry);


inline void writeImm32(ProgramMemory& pm, uint64_t& pc, uint32_t v) {
    pm.write(pc++, static_cast<uint8_t>(TypeData::Data));
    pm.write(pc++, 4);
    pm.write(pc++, (v >> 0) & 0xFF);
    pm.write(pc++, (v >> 8) & 0xFF);
    pm.write(pc++, (v >> 16) & 0xFF);
    pm.write(pc++, (v >> 24) & 0xFF);
}

struct CUEnv {
    Bus bus;
    ControlUnit cu;
    ProgramMemory pm;

    
    RegisterBlockDevice gpr;
    MultiByteMemoryDevice dataMem;
    ALUDevice_Synchron alu;

    uint64_t gprBase = 0x100;
    uint64_t dataBase = 0x200;
    uint64_t aluBase = 0x300;

    CUEnv(size_t pmSize = 256)
        : bus(0x1000)
        , cu()
        , pm(pmSize)
        , gpr(128)
        , dataMem(256)
        , alu(1)
    {
        cu.setBus(&bus);
        cu.setProgramMemory(&pm);

        bus.attachDevice("GPR", gprBase, 128, &gpr);
        bus.attachDevice("DataMemory", dataBase, 256, &dataMem);
        bus.attachDevice("ALU", aluBase, 64, &alu);

        registerAllCommands(cu.getRefDecoder().getFactory());
    }
};
