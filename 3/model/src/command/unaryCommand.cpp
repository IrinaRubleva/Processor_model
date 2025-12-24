#include "../include/commands/UnaryCommand.hpp"
#include "../include/commands/command.hpp"
#include "../include/controleUnit.hpp"
#include "../../include/device/alu.hpp"


std::unique_ptr<Command> ConcreteCommandFactory::createUnary(sizeOfOpcode Opcode, const OperandDescriptor &operand)
{
    if (Opcode == OpcodeMap["INC"])
    {
        //  OpcodeCommandMap[Opcode] = std::make_unique<IncCommand>(Opcode, operand);
        return std::make_unique<IncCommand>(Opcode, operand);
    }
    else if (Opcode == OpcodeMap["DEC"])
    {
        /// OpcodeCommandMap[Opcode] = std::make_unique<DecCommand>(Opcode, operand);
        return std::make_unique<DecCommand>(Opcode, operand);
    }
    else if (Opcode == OpcodeMap["NEG"])
    {
        // OpcodeCommandMap[Opcode] = std::make_unique<NegCommand>(Opcode, operand);
        return std::make_unique<NegCommand>(Opcode, operand);
    }
    else if (Opcode == OpcodeMap["NOT"])
    {
        // OpcodeCommandMap[Opcode] = std::make_unique<NotCommand>(Opcode, operand);
        return std::make_unique<NotCommand>(Opcode, operand);
    }
    else
        throw std::runtime_error("Not a unary operation");
}

void UnaryCommand::compute(ControlUnit &cu)
{
    std::cout << "[UnaryCommand::compute] START for command type" << std::endl;
    
    auto OpcodeMap = cu.getDecoder()->getFactory().OpcodeMap;
    Bus *busPtr = cu.getBus();
    Bus &bus = *busPtr;

    std::cout << "[UnaryCommand::compute] Opcode: " << int(opcode) << std::endl;

  
    std::string name;
    for (const auto &[cmdName, code] : OpcodeMap)
    {
        if (code == opcode)
        {
            name = cmdName;
            break;
        }   
    }
    std::cout << "[UnaryCommand::compute] Command name: " << name << std::endl;
    
    
    uint64_t aluBase = cu.getDeviceAddress("ALU");
    bus.write(aluBase + 0, (uint8_t)(opcode));

    uint64_t A = operand.getRawValue(bus);

    std::cout << "[UnaryCommand::compute] A=" << A << std::endl;

    for (uint8_t i = 0; i < 8; ++i)
    {
        bus.write(aluBase + 1 + i, (A >> (i * 8)) & 0xFF);
    }
    

    for (uint8_t i = 0; i < 8; ++i)
    {
        bus.write(aluBase + 9 + i, 0);
    }

    if (auto aluDev = cu.getBus()->getDevice<ReadableDevice>("ALU"))
    {
        if (auto syncAlu = dynamic_cast<ALUDevice_Synchron*>(*aluDev))
        {
            // Передаем команду в ALU
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

    std::cout << "[UnaryCommand::compute] Result from ALU: " << result << std::endl;

    operand.setRawValue(bus, result);
}
