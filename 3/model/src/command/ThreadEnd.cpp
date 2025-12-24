#include "../include/commands/ThreadEnd.hpp"
#include "../include/commands/command.hpp"   
#include "../include/controleUnit.hpp"
    
std::unique_ptr<Command> ConcreteCommandFactory:: createThreadEnd(sizeOfOpcode Opcode, const OperandDescriptor& operand){
    if (Opcode == OpcodeMap["THRE"]){
       // OpcodeCommandMap[Opcode] = std::make_unique<ThreadEndCommand>(Opcode, operand);
        return std::make_unique<ThreadEndCommand>(Opcode, operand);
    }
    else    throw std::runtime_error("Not a threadEnd operation");
}




void ThreadEndCommand::compute(ControlUnit& cu) {
    Bus* busPtr = cu.getBus();
    Bus& bus = *busPtr; 

    uint64_t gprBase = bus.getDeviceAddress("GPR");

    uint32_t value = 0;
    for (int i = 0; i < 4; ++i) {
        value |= (uint32_t)bus.read(gprBase + i) << (i * 8);
    }
    if (value > 0) {
        value--;
    }
    for (int i = 0; i < 4; ++i) {
        bus.write(gprBase + i, (value >> (i * 8)) & 0xFF);
    }
}
