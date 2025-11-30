#include "../../include/device/alu.hpp"

uint8_t ALUDevice_Synchron:: read(uint16_t port){
    if (port == 0)      return static_cast<uint8_t>(opCode);
    if (port >= 1 && port <= 8)    return operandA[port - 1];
    if (port >= 9 && port <= 16)    return operandB[port - 9];
    if (port >= 17 && port <= 24)   return result[port - 17];
    if (port >= 25 && port <= 32)   return flags[port - 25];
    return 0;
}

void ALUDevice_Synchron:: write(uint16_t port, uint8_t value){;
    if (port == 0) {
        opCode = static_cast<OperationType>(value);
        is_queueExists = true;
        return;}

    if (port >= 1 && port <= 8) {
        operandA[port - 1] = value;
        is_queueExists = true;
        return;}

    if (port >= 9 && port <= 16) {
        operandB[port - 9] = value;
        is_queueExists = true;
        return;}
}


void ALUDevice_Synchron:: tick(){
    if(!is_queueExists) return;
    
    is_queueExists = false;

    uint64_t A = unpack(operandA);
    uint64_t B = unpack(operandB);

    auto [R, F] = compute(opCode, A, B);

    pack(result, R);
    pack(flags, F);
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

std::pair<uint64_t,uint64_t> ALUDevice_Synchron:: compute(OperationType op, uint64_t A, uint64_t B){
    uint64_t R=0, F=0;
    switch (op) {
        case OperationType::ADD:
            R = A + B;
            F = (R < A);
            break;
        case OperationType::SUB:
            R = A - B;
            F = (A < B);
            break;
        case OperationType::INC:
            R = A + 1;
            break;
        case OperationType::DEC:
            R = A - 1;
            break;
        case OperationType::NEG:
            R = (uint64_t)(-(int64_t)A);
            break;
        case OperationType::AND: R = A & B; break;
        case OperationType::OR:  R = A | B; break;
        case OperationType::XOR: R = A ^ B; break;
        case OperationType::MUL: R = A * B; break;
        case OperationType::DIV:
            if (B == 0) { R = 0; F = 1; }
            else { R = A / B; F = A % B; }
            break;
        default:
            F = 1;
    }
    return {R,F};
}



//-----------------------ALU_Asynchron---------------------------------------

uint8_t ALUDevice_Asynchron:: read(uint16_t port){
    if (port == 0)      return static_cast<uint8_t>(opCode);
    if (port >= 1 && port <= 8)    return operandA[port - 1];
    if (port >= 9 && port <= 16)    return operandB[port - 9];
    if (port >= 17 && port <= 24)   return result[port - 17];
    if (port >= 25 && port <= 32)   return flags[port - 25];
    return 0;
}

void ALUDevice_Asynchron:: write(uint16_t port, uint8_t value){;
    if (port == 0) {
        opCode = static_cast<OperationType>(value);
        is_Ready = true;
        tryExecute();  //искусственная задержка
        return;}

    if (port >= 1 && port <= 8) {
        operandA[port - 1] = value;
        is_Ready = true;
        tryExecute();  //искусственная задержка
        return;}

    if (port >= 9 && port <= 16) {
        operandB[port - 9] = value;
        is_Ready = true;
        tryExecute();  //искусственная задержка
        return;}
}

void ALUDevice_Asynchron:: tryExecute(){
    if (!is_Ready) {return;}
        uint64_t A = unpack(operandA);
    uint64_t B = unpack(operandB);

    auto [R, F] = compute(opCode, A, B);

    pack(result, R);
    pack(flags,  F);

    is_Ready = true;
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

std::pair<uint64_t,uint64_t> ALUDevice_Asynchron:: compute(OperationType op, uint64_t A, uint64_t B){
    uint64_t R=0, F=0;
    switch (op) {
        case OperationType::ADD:
            R = A + B;
            F = (R < A);
            break;
        case OperationType::SUB:
            R = A - B;
            F = (A < B);
            break;
        case OperationType::INC:
            R = A + 1;
            break;
        case OperationType::DEC:
            R = A - 1;
            break;
        case OperationType::NEG:
            R = (uint64_t)(-(int64_t)A);
            break;
        case OperationType::AND: R = A & B; break;
        case OperationType::OR:  R = A | B; break;
        case OperationType::XOR: R = A ^ B; break;
        case OperationType::MUL: R = A * B; break;
        case OperationType::DIV:
            if (B == 0) { R = 0; F = 1; }
            else { R = A / B; F = A % B; }
            break;
        default:
            F = 1;
    }
    return {R,F};
}