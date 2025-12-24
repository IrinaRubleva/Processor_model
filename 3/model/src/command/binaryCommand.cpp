#include "../include/commands/BinaryCommand/_BinaryCommand.hpp"
#include "../include/commands/command.hpp"
#include "../include/controleUnit.hpp"
#include "../../include/device/alu.hpp"
#include "../../include/operand.hpp"


void BinaryCommand::compute(ControlUnit & cu){
    Bus& bus = cu.getRefBus();
    
    auto& factory = cu.getRefDecoder().getFactory();
    
    std::string cmdName;
    try {
        cmdName = factory.getName(Opcode);
    } catch (const std::exception& e) {
        std::cerr << "[BinaryCommand::compute] Error getting command name: " << e.what() << std::endl;
        throw;
    }
    
    // ======== MOV ========
    if (cmdName == "MOV") {
        uint64_t sourceValue = operand2.getRawValue(bus);
        operand1.setRawValue(bus, sourceValue);
        return;  
    }
    
    // ======== CMP ========
    if (cmdName == "CMP") {
        uint64_t a = operand1.getRawValue(bus);
        uint64_t b = operand2.getRawValue(bus);
        uint64_t result = (a == b) ? 1 : 0;
        
        operand1.setRawValue(bus, result);
        return;
    }
    
    uint64_t aluBase = cu.getDeviceAddress("ALU");
    bus.write(aluBase + 0, static_cast<uint8_t>(Opcode));
    
    uint64_t A = operand1.getRawValue(bus); 
    uint64_t B = operand2.getRawValue(bus); 
    
    for (uint8_t i = 0; i < 8; ++i) {
        bus.write(aluBase + 1 + i, (A >> (i * 8)) & 0xFF);
        bus.write(aluBase + 9 + i, (B >> (i * 8)) & 0xFF);
    }
    
    if (auto aluDev = cu.getBus()->getDevice<ReadableDevice>("ALU")) {
        if (auto syncAlu = dynamic_cast<ALUDevice_Synchron*>(*aluDev)) {
     
            syncAlu->setCurrentCommand(this);
            syncAlu->tick(cu);
            syncAlu->setCurrentCommand(nullptr);
        }
    }
    
    uint64_t result = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t byteVal = bus.read(aluBase + 17 + i);
        result |= (uint64_t)byteVal << (i * 8);
    }
    operand1.setRawValue(bus, result);

}