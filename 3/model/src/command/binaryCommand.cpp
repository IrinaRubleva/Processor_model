#include "../include/commands/BinaryCommand.hpp"
#include "../include/commands/command.hpp"
#include "../include/controleUnit.hpp"
#include "../../include/device/alu.hpp"



std::unique_ptr<Command> ConcreteCommandFactory:: createBinary
(sizeOfOpcode Opcode, const OperandDescriptor& left, const OperandDescriptor& right){
        if (Opcode == OpcodeMap["ADD"]){
           // OpcodeCommandMap[Opcode] = std::make_unique<AddCommand>(Opcode, left, right);
            return std::make_unique<AddCommand>(Opcode, left, right);
        }
        else if (Opcode == OpcodeMap["SUB"]){
            //OpcodeCommandMap[Opcode] = std::make_unique<SubCommand>(Opcode, left, right);
            return std::make_unique<SubCommand>(Opcode, left, right);
        }
        else if (Opcode == OpcodeMap["MUL"]){
            //OpcodeCommandMap[Opcode] = std::make_unique<MulCommand>(Opcode, left, right);
            return std::make_unique<MulCommand>(Opcode, left, right);
        }
        else if (Opcode == OpcodeMap["DIV"]){
           // OpcodeCommandMap[Opcode] = std::make_unique<DivCommand>(Opcode, left, right);
            return std::make_unique<DivCommand>(Opcode, left, right);
        }
        else if (Opcode == OpcodeMap["CMP"]){
          //  OpcodeCommandMap[Opcode] = std::make_unique<CmpCommand>(Opcode, left, right);
            return std::make_unique<CmpCommand>(Opcode, left, right);
        }
        else if (Opcode == OpcodeMap["MOV"]){
          //  OpcodeCommandMap[Opcode] = std::make_unique<MovCommand>(Opcode, left, right);
            return std::make_unique<MovCommand>(Opcode, left, right);
        }
        else    throw std::runtime_error("Not a binary operation");
}

void BinaryCommand::compute(ControlUnit & cu){
    std::cout << "[BinaryCommand::compute] START for command type" << std::endl;
    auto OpcodeMap = cu.getDecoder()->getFactory().OpcodeMap;
    Bus* busPtr = cu.getBus();
    Bus& bus = *busPtr;
    
    std::cout << "[BinaryCommand::compute] Opcode: " << int(Opcode) << std::endl;
    
    // Определим имя команды
    std::string name;
    for (const auto &[cmdName, code] : OpcodeMap) {
        if (code == Opcode) {
            name = cmdName;
            break;
        }
    }
    std::cout << "[BinaryCommand::compute] Command name: " << name << std::endl;
    
    // ======== MOV ========
    if (Opcode == OpcodeMap["MOV"]) {
        std::cout << "[BinaryCommand::compute] MOV operation" << std::endl;
        
        uint64_t sourceValue = operand2.getRawValue(bus);
        std::cout << "[BinaryCommand::compute] sourceValue: " << sourceValue << std::endl;
        
        // ИСПРАВЛЕНО: используем setRawValue, который сам знает куда писать!
        operand1.setRawValue(bus, sourceValue);
        return;  
    }
    
    // ======== CMP ========
    if (Opcode == OpcodeMap["CMP"]) {
        std::cout << "[BinaryCommand::compute] CMP operation" << std::endl;
        
        uint64_t a = operand1.getRawValue(bus);
        uint64_t b = operand2.getRawValue(bus);
        uint64_t result = (a == b) ? 1 : 0;
        
        // ИСПРАВЛЕНО: используем setRawValue
        operand1.setRawValue(bus, result);
        return;
    }
    
    // ======== ALU операции (ADD, SUB, MUL, DIV) ========
    std::cout << "[BinaryCommand::compute] ALU operation: " << name << std::endl;
    
    uint64_t aluBase = cu.getDeviceAddress("ALU");
    bus.write(aluBase + 0, static_cast<uint8_t>(Opcode));
    
    uint64_t A = operand1.getRawValue(bus); 
    uint64_t B = operand2.getRawValue(bus); 
    
    std::cout << "[BinaryCommand::compute] A=" << A << ", B=" << B << std::endl;

    for (uint8_t i = 0; i < 8; ++i) {
        bus.write(aluBase + 1 + i, (A >> (i * 8)) & 0xFF);
        bus.write(aluBase + 9 + i, (B >> (i * 8)) & 0xFF);
    }
    
    if (auto aluDev = cu.getBus()->getDevice<ReadableDevice>("ALU")) {
        if (auto syncAlu = dynamic_cast<ALUDevice_Synchron*>(*aluDev)) {
     
            // Передаем текущую команду в ALU!
            syncAlu->setCurrentCommand(this);
            
            // Запускаем ALU
            syncAlu->tick(cu);
            
            // Очищаем ссылку
            syncAlu->setCurrentCommand(nullptr);
        }
    }
    
    uint64_t result = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t byteVal = bus.read(aluBase + 17 + i);
        result |= (uint64_t)byteVal << (i * 8);
    }
    
    std::cout << "[BinaryCommand::compute] Result from ALU: " << result << std::endl;
    
    // ИСПРАВЛЕНО: используем setRawValue для записи результата
    operand1.setRawValue(bus, result);
}



// // BinaryCommand.cpp - compute() метод
// void BinaryCommand::compute(ControlUnit & cu){
//     Bus* busPtr = cu.getBus();
//     Bus& bus = *busPtr;
    
//     uint64_t aluBase = cu.getDeviceAddress("ALU");
//     bus.write(aluBase + 0, static_cast<uint8_t>(Opcode));
    
//     uint64_t A = operand1.getRawValue(bus); 
//     uint64_t B = operand2.getRawValue(bus); 
    
//     std::cout << "[BinaryCommand::compute] A=" << A << ", B=" << B << std::endl;

//     // Записываем операнды в ALU
//     for (uint8_t i = 0; i < 8; ++i) {
//         bus.write(aluBase + 1 + i, (A >> (i * 8)) & 0xFF);
//         bus.write(aluBase + 9 + i, (B >> (i * 8)) & 0xFF);
//     }
    
//     // Получаем устройство ALU
//     if (auto aluDev = cu.getBus()->getDevice<ReadableDevice>("ALU")) {
//         if (auto syncAlu = dynamic_cast<SynchronDevice*>(*aluDev)) {
     
//             // Передаем текущую команду в ALU!
//             syncAlu->setCurrentCommand(this);
            
//             // Запускаем ALU
//             syncAlu->tick(cu);
            
//             // Очищаем ссылку
//             syncAlu->setCurrentCommand(nullptr);
//         }
//     }
    
//     // Читаем результат из ALU
//     uint64_t result = 0;
//     for (uint8_t i = 0; i < 8; ++i) {
//         uint8_t byteVal = bus.read(aluBase + 17 + i);
//         result |= (uint64_t)byteVal << (i * 8);
//     }
    
//     std::cout << "[BinaryCommand::compute] Result from ALU: " << result << std::endl;
    
//     // Записываем результат в первый операнд
//     operand1.setRawValue(bus, result);
// }