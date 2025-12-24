#include "../include/commands/DataDeclaration.hpp"
#include "../include/commands/command.hpp"
#include "../include/controleUnit.hpp"
#include "../include/operand.hpp"

std::unique_ptr<Command> ConcreteCommandFactory::createDataDeclaration(sizeOfOpcode Opcode, const OperandDescriptor &operand, uint64_t offset)
{
    if (Opcode == OpcodeMap["DW"])
    {
        // OpcodeCommandMap[Opcode] = std::make_unique<DwDataDeclarationCommand>(Opcode, operand, offset);
        return std::make_unique<DwDataDeclarationCommand>(Opcode, operand, offset);
    }
    else if (Opcode == OpcodeMap["DB"])
    {
        //  OpcodeCommandMap[Opcode] = std::make_unique<DbDataDeclarationCommand>(Opcode, operand, offset);
        return std::make_unique<DbDataDeclarationCommand>(Opcode, operand, offset);
    }
    else if (Opcode == OpcodeMap["DQ"])
    {
        ///  OpcodeCommandMap[Opcode] = std::make_unique<DqDataDeclarationCommand>(Opcode, operand, offset);
        return std::make_unique<DqDataDeclarationCommand>(Opcode, operand, offset);
    }
    else if (Opcode == OpcodeMap["DD"])
    {
        // OpcodeCommandMap[Opcode] = std::make_unique<DdDataDeclarationCommand>(Opcode, operand, offset);
        return std::make_unique<DdDataDeclarationCommand>(Opcode, operand, offset);
    }
    else
        throw std::runtime_error("Not a dataDeclaration operation");
}


// void DataDeclarationCommand::compute(ControlUnit &cu)
// {
//     Bus &bus = cu.getRefBus();
//     uint64_t base = bus.getDeviceAddress("DataMemory");

//     uint64_t offset = operand.getSize();
//     uint64_t value = operand.getRawValue(bus);

//     for (uint8_t i = 0; i < operand.getSize(); ++i)
//     {
//         bus.write(base + offset + i, (value >> (i * 8)) & 0xFF);
//     }
// }

void DataDeclarationCommand::compute(ControlUnit &cu)
{
    Bus &bus = cu.getRefBus();
    uint64_t base = bus.getDeviceAddress("DataMemory");
    
    uint64_t value = operand.getValue();  // значение для записи
    uint8_t size = operand.getSize();     // размер
    
    std::cout << "[DataDeclarationCommand] Writing value=" << value 
              << " size=" << (int)size << " to offset=" << offset << std::endl;
    
    for (uint8_t i = 0; i < size; ++i) {
        uint8_t byteValue = (value >> (i * 8)) & 0xFF;
        bus.write(base + offset + i, byteValue);
    }
}