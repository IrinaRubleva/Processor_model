#include "../include/commands/UnaryCommand/_UnaryCommand.hpp"

#include "../include/controleUnit.hpp"
#include "../../include/device/alu.hpp"


void UnaryCommand::compute(ControlUnit &cu)
{
    Bus& bus = cu.getRefBus();
    
    auto& factory = cu.getRefDecoder().getFactory();
    
    std::string cmdName;
    try {
        cmdName = factory.getName(opcode);
    } catch (const std::exception& e) {
        std::cerr << "[UnaryCommand::compute] Error getting command name: " << e.what() << std::endl;
        throw;
    }
    
    
    uint64_t aluBase = cu.getDeviceAddress("ALU");
    bus.write(aluBase + 0, (uint8_t)(opcode));

    uint64_t A = operand.getRawValue(bus);

    for (uint8_t i = 0; i < 8; ++i)
    {
        bus.write(aluBase + 1 + i, (A >> (i * 8)) & 0xFF);
    }
    

    for (uint8_t i = 0; i < 8; ++i)
    {
        bus.write(aluBase + 9 + i, 0);
    }

    if (auto aluDev = bus.getDevice<ReadableDevice>("ALU"))
    {
        if (auto syncAlu = dynamic_cast<ALUDevice_Synchron*>(*aluDev))
        {
            syncAlu->setCurrentCommand(this);
            syncAlu->tick(cu);
            syncAlu->setCurrentCommand(nullptr);
        }
    }

    uint64_t result = 0;
    for (uint8_t i = 0; i < 8; ++i)
    {
        uint8_t byteVal = bus.read(aluBase + 17 + i);
        result |= (uint64_t)byteVal << (i * 8);
    }

    operand.setRawValue(bus, result);
}