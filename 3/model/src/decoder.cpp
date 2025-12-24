#include "../include/decoder.hpp"
#include "programMemory.hpp"
#include "../include/commands/command.hpp"
#include <iostream>

std::unique_ptr<Command> Decoder::decode(uint64_t &pc, ProgramMemory &mem)
{
    uint8_t opc = mem.read(pc++);

    if (!factory->OpcodeCommandMap.count(opc))
    {
        std::cout << "[Decoder] Unknown opcode " << int(opc) << "\n";
        return nullptr;
    }


    std::string name;
    for (const auto &[cmdName, code] : factory->OpcodeMap)
    {
        if (code == opc)
        {
            name = cmdName;
            break;
        }
    }
    std::cout << "[Decoder::decode] Command: " << name << " (opcode=" << int(opc) << ")" << std::endl;

    uint8_t count = mem.read(pc++);
    OperandDescriptor op1, op2;

    auto readOp = [&](OperandDescriptor &o)
    {
        uint8_t type = mem.read(pc++);
        uint8_t size = mem.read(pc++);
        uint64_t v = 0;
        for (int i = 0; i < size; ++i)
            v |= (uint64_t)mem.read(pc++) << (i * 8);
        o = OperandDescriptor((TypeData)type, size, v);
    };

    if (count >= 1)
        readOp(op1);
    if (count == 2)
        readOp(op2);

    if (name == "DB" || name == "DW" || name == "DD" || name == "DQ")
    {
        std::cout << "[Decoder::decode] Creating data declaration: " << name << std::endl;

        if (count != 2)
        {
            std::cout << "[Decoder::decode] ERROR: Data declaration should have 2 operands, got "
                      << (int)count << std::endl;
            return nullptr;
        }

        std::cout << "[Decoder::decode] Operand1 type=" << (int)op1.getType() << ", Operand2 type=" << (int)op2.getType() << std::endl;

        if (op1.getType() != TypeData::Address)
        {
            std::cout << "[Decoder::decode] WARNING: First operand should be Address, got "
                      << (int)op1.getType() << std::endl;
        }

        if (op2.getType() != TypeData::Data)
        {
            std::cout << "[Decoder::decode] WARNING: Second operand should be Data, got "
                      << (int)op2.getType() << std::endl;
        }

     
        uint64_t offset = op1.getValue(); // offset уже в value поле

        std::cout << "[Decoder::decode] Data: offset=" << offset << ", value=" << op2.getValue() << ", size=" << (int)op2.getSize() << std::endl;

 
        return factory->createDataDeclaration(opc, op2, offset);
    }
    

    if (name == "INC" || name == "DEC" || name == "NEG" || name == "NOT")
    {
        std::cout << "[Decoder::decode] It's a unary command!" << std::endl;
        return factory->createUnary(opc, op1);
    }

    if (name == "JMP")
        return factory->createJump(opc, op1);

    if (name == "THRI")
        return factory->createThreadInit(opc, op1);

    if (name == "THRE")
        return factory->createThreadEnd(opc, op1);

    if (name == "ADD" || name == "SUB" || name == "MUL" ||
        name == "DIV" || name == "CMP" || name == "MOV" || name == "JE")
        return factory->createBinary(opc, op1, op2);

   

    std::cout << "[Decoder] Unknown command type: " << name << "\n";
    return nullptr;
}
