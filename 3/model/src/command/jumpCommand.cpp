#include "../include/commands/JumpCommand.hpp"
#include "../include/commands/command.hpp"
#include "../include/operand.hpp"
#include "../include/controleUnit.hpp"



#include <iostream>
std::unique_ptr<Command> ConcreteCommandFactory::createJump(sizeOfOpcode Opcode, const OperandDescriptor &operand)
{
    if (Opcode == OpcodeMap["JMP"])
    {
       // OpcodeCommandMap[Opcode] = std::make_unique<JmpCommand>(Opcode, operand);
        return std::make_unique<JmpCommand>(Opcode, operand);
    }
    else
        throw std::runtime_error("Not a jump operation");
}

// по лейблу
void JumpCommand::compute(ControlUnit &cu)
{
    uint64_t addr = operand.getValue(); 
    
    std::cout << "[JumpCommand] Jumping to ProgramMemory address: " << addr << std::endl;
    
    cu.setPC(addr);
}