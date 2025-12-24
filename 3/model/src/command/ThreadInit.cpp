#include "../include/commands/ThreadInit.hpp"
#include "../include/commands/command.hpp"   
#include "../include/controleUnit.hpp"


std::unique_ptr<Command> ConcreteCommandFactory:: createThreadInit(sizeOfOpcode Opcode, const OperandDescriptor& operand){
    if (Opcode == OpcodeMap["THRI"]){
      //  OpcodeCommandMap[Opcode] = std::make_unique<ThreadInitCommand>(Opcode, operand);
        return std::make_unique<ThreadInitCommand>(Opcode, operand);
    }
    else    throw std::runtime_error("Not a threadInit operation");
}



void ThreadInitCommand::compute(ControlUnit& cu) {
    Bus* busPtr = cu.getBus();
    Bus& bus = *busPtr; 


    uint64_t gprBase = bus.getDeviceAddress("GPR");
    uint32_t value = 0;

    for (int i = 0; i < 4; ++i) {
        value |= (uint32_t)bus.read(gprBase + i) << (i * 8);
    }
    value++;
    for (int i = 0; i < 4; ++i) {
        bus.write(gprBase + i, (value >> (i * 8)) & 0xFF);
    }
}
