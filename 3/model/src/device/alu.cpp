#include "../../include/device/alu.hpp"
#include "../../include/operand.hpp"
#include "../../include/commands/command.hpp"
#include "../../include/controleUnit.hpp"
#include "../../include/commands/BinaryCommand/_BinaryCommand.hpp"
#include "../../include/commands/UnaryCommand/_UnaryCommand.hpp"
#include <iostream>
typedef uint64_t port_size;


uint8_t ALUDevice_Synchron::read(port_size port) const {
    std::lock_guard<std::mutex> lock(state_mtx);

    if (port == 0) return opCode;
    if (port >= 1 && port <= 8) return operandA[port - 1];
    if (port >= 9 && port <= 16) return operandB[port - 9];
    if (port >= 17 && port <= 24) return result[port - 17];
    if (port >= 25 && port <= 32) return flags[port - 25];
    return 0xFF;
}


void ALUDevice_Synchron::write(port_size port, uint8_t value) {
    std::lock_guard<std::mutex> lock(state_mtx);
    if (port == 0) {
        opCode = value;
        return;
    }

    if (port >= 1 && port <= 8) {
        operandA[port - 1] = value;
        return;
    }

    if (port >= 9 && port <= 16) {
        operandB[port - 9] = value;
        return;
    }
}



void ALUDevice_Synchron::tick(ControlUnit& cu)
{
    //копирую
    Command* cmd;
    uint8_t localOp;
    std::array<uint8_t,8> A, B;

    {
        std::lock_guard<std::mutex> lock(state_mtx);
        cmd = currentCommand;
        localOp = opCode;
        A = operandA;
        B = operandB;
    }

    if (!cmd)
        return;

    //вычисляю
    uint64_t a = unpack(A);
    uint64_t b = unpack(B);
    uint64_t resultValue = 0;

    if (auto* binaryCmd = dynamic_cast<BinaryCommand*>(cmd)) {
        OperandDescriptor orig1 = binaryCmd->operand1;
        OperandDescriptor orig2 = binaryCmd->operand2;

        binaryCmd->operand1 = OperandDescriptor(TypeData::Data, 8, a);
        binaryCmd->operand2 = OperandDescriptor(TypeData::Data, 8, b);

        binaryCmd->execute(cu);
        resultValue = binaryCmd->operand1.getValue();

        binaryCmd->operand1 = orig1;
        binaryCmd->operand2 = orig2;
    }
    else if (auto* unaryCmd = dynamic_cast<UnaryCommand*>(cmd)) {
        OperandDescriptor orig = unaryCmd->operand;

        unaryCmd->operand = OperandDescriptor(TypeData::Data, 8, a);
        unaryCmd->execute(cu);
        resultValue = unaryCmd->operand.getValue();

        unaryCmd->operand = orig;
    }

    // запись
    {
        std::lock_guard<std::mutex> lock(state_mtx);
        pack(result, resultValue);
        currentCommand = nullptr;
    }
}



uint64_t ALUDevice_Synchron:: unpack(const std::array<uint8_t,8>& a){
    uint64_t x=0;
    for(int i=0;i<8;i++)
        x |= (uint64_t)a[i] << (i*8);
    return x;
}

void ALUDevice_Synchron:: pack(std::array<uint8_t,8>& dst, uint64_t val){
    for(int i=0;i<8;i++)
        dst[i] = (val >> (i*8)) & 0xFF;
}




//-----------------------ALU_Asynchron---------------------------------------

uint8_t ALUDevice_Asynchron:: read(port_size port) const{
    if (port == 0)      return static_cast<uint8_t>(opCode);
    if (port >= 1 && port <= 8)    return operandA[port - 1];
    if (port >= 9 && port <= 16)    return operandB[port - 9];
    if (port >= 17 && port <= 24)   return result[port - 17];
    if (port >= 25 && port <= 32)   return flags[port - 25];
    return 0;
}

void ALUDevice_Asynchron::write(port_size port, uint8_t value) {
    if (port == 0) {
        opCode = value;
        std::cout << "[ALU_Async::write] Set opCode=" << (int)value << std::endl;
        return;
    }

    if (port >= 1 && port <= 8) {
        operandA[port - 1] = value;
        std::cout << "[ALU_Async::write] operandA[" << (port-1) << "] = 0x" 
                  << std::hex << (int)value << std::dec << std::endl;
        return;
    }

    if (port >= 9 && port <= 16) {
        operandB[port - 9] = value;
        std::cout << "[ALU_Async::write] operandB[" << (port-9) << "] = 0x" 
                  << std::hex << (int)value << std::dec << std::endl;
        
        
        if (port == 16) {
            is_Ready = true;
            std::cout << "[ALU_Async::write] Set is_Ready = true" << std::endl;
        }
        return;
    }
}

void ALUDevice_Asynchron:: tick(ControlUnit& cu){
    std::cout << "=== [ALU_Async::tick] START ===" << std::endl;
    std::cout << "[ALU_Async::tick] is_Ready=" << is_Ready << std::endl;
    std::cout << "[ALU_Async::tick] currentCommand=" << currentCommand << std::endl;
    std::cout << "[ALU_Async::tick] opCode=" << (int)opCode << std::endl;
    
    if (!is_Ready) {
        std::cout << "[ALU_Async::tick] NOT READY, returning" << std::endl;
        return;
    }

    uint64_t A = unpack(operandA);
    uint64_t B = unpack(operandB);
    
    std::cout << "[ALU_Async::tick] A=" << A << ", B=" << B << std::endl;
    
    if (auto* binaryCmd = dynamic_cast<BinaryCommand*>(currentCommand)) {
        std::cout << "[ALU_Async::tick] Binary command detected" << std::endl;
        
        
        OperandDescriptor originalOp1 = binaryCmd->operand1;
        OperandDescriptor originalOp2 = binaryCmd->operand2;
        
        std::cout << "[ALU_Async::tick] Original op1 value: " << originalOp1.getValue() << std::endl;
        std::cout << "[ALU_Async::tick] Original op2 value: " << originalOp2.getValue() << std::endl;
        
        OperandDescriptor tempOp1(TypeData::Data, 8, A);
        OperandDescriptor tempOp2(TypeData::Data, 8, B);
        
        std::cout << "[ALU_Async::tick] tempOp1.value=" << tempOp1.getValue() 
                  << ", tempOp2.value=" << tempOp2.getValue() << std::endl;
        
        binaryCmd->operand1 = tempOp1;
        binaryCmd->operand2 = tempOp2;
        
        
        std::cout << "[ALU_Async::tick] Calling execute()..." << std::endl;
        binaryCmd->execute(cu); 
        
    
        uint64_t resultValue = binaryCmd->operand1.getValue();
        
        std::cout << "[ALU_Async::tick] After execute: binaryCmd->operand1.value=" 
                  << binaryCmd->operand1.getValue() 
                  << ", resultValue=" << resultValue << std::endl;
        
        pack(result, resultValue);
        std::cout << "[ALU_Async::tick] Packed result: " << resultValue << std::endl;
        
       
        binaryCmd->operand1 = originalOp1;
        binaryCmd->operand2 = originalOp2;
    }
    else if (auto* unaryCmd = dynamic_cast<UnaryCommand*>(currentCommand)) {
        OperandDescriptor originalOp = unaryCmd->operand;
        
        OperandDescriptor tempOp(TypeData::Data, 8, A);
        
  //      std::cout << "[ALU::tick] Before execute (unary): tempOp.value=" << tempOp.getValue() << std::endl;
       
        unaryCmd->operand = tempOp;
        
        unaryCmd->execute(cu);
        
        uint64_t resultValue = unaryCmd->operand.getValue();
        
        // std::cout << "[ALU::tick] After execute (unary): unaryCmd->operand.value=" 
        //           << unaryCmd->operand.getValue() 
        //           << ", resultValue=" << resultValue << std::endl;
        
        pack(result, resultValue);
        
        unaryCmd->operand = originalOp;
    }
    is_Ready = false;
}

uint64_t ALUDevice_Asynchron:: unpack(const std::array<uint8_t,8>& a){
    uint64_t x=0;
    for(int i=0;i<8;i++)
        x |= (uint64_t)a[i] << (i*8);
    return x;
}

void ALUDevice_Asynchron:: pack(std::array<uint8_t,8>& dst, uint64_t val){
    for(int i=0;i<8;i++)
        dst[i] = (val >> (i*8)) & 0xFF;
}
