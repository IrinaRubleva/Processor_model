#include "../include/decoder.hpp"
#include "programMemory.hpp"
#include <iostream>

std::unique_ptr<Command> Decoder::decode(uint64_t& pc, ProgramMemory& mem)
{
    uint8_t opcode = mem.read(pc++);
    if (!factory->isValidOpcode(opcode)) {
        return nullptr;
    }

    uint8_t count = mem.read(pc++);
    std::vector<OperandDescriptor> operands;
    operands.reserve(count);

    for (uint8_t i = 0; i < count; ++i) {
        uint8_t type = mem.read(pc++);
        uint8_t size = mem.read(pc++);
        uint64_t value = 0;
        for (uint8_t b = 0; b < size; ++b)
            value |= uint64_t(mem.read(pc++)) << (8 * b);

        operands.emplace_back(static_cast<TypeData>(type), size, value);
    }


    return factory->createCommand(opcode, operands, pc);
    
    for (size_t i = 0; i < operands.size(); ++i) {
        // std::cout << "[Decoder] Operand " << i << ": type=" << (int)operands[i].getType()
        //           << ", size=" << (int)operands[i].getSize()
        //           << ", value=" << operands[i].getValue() << std::endl;
    }
}
