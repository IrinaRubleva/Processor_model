#pragma once
#include "command.hpp"
#include "programMemory.hpp"
#include <memory>

//определить тип комманды
class Decoder{
public:
    std::unique_ptr<Command> decode(uint64_t& pc, ProgramMemory& mem);
private:
    CommandFactory* factory;
}; 


std::unique_ptr<Command> Decoder::decode(uint64_t& pc, ProgramMemory& mem) {
    uint8_t opcode = mem.fetchByte(pc++);
    auto iter = instructionTable.find(opcode);
    if (iter == instructionTable.end())
        throw std::runtime_error("Unknown opcode");

    OperationType op = iter->second;

    return factory->create(op, pc, mem);
}