#include "../include/commands/DataDeclaration/_DataDeclaration.hpp"
#include "../include/commands/command.hpp"
#include "../include/controleUnit.hpp"
#include "../include/operand.hpp"


void DataDeclarationCommand::compute(ControlUnit& cu) {
    Bus &bus = cu.getRefBus();
    
    uint64_t base;
    try {
        base = bus.getDeviceAddress("DataMemory");
    } catch (const std::exception& e) {
        return;
    }
    
    
    uint64_t packed = operand.getValue();
    uint64_t offset = packed >> 8;
    uint8_t expectedSize = packed & 0xFF;
    
    
    uint8_t actualSize = valueOperand.getSize();
    uint64_t value = valueOperand.getValue();
    
    
   
    for (uint8_t i = 0; i < actualSize; ++i) {
        uint8_t byteValue = (value >> (i * 8)) & 0xFF;
        uint64_t address = base + offset + i;
        bus.write(address, byteValue);
        }
}