#pragma once

#include "../command.hpp"


/*!
*@brief Описатель объявления данных. Результат выполнения операции помещается в первый операнд.
*/


class DataDeclarationCommand: public Command{
protected:
    sizeOfOpcode opcode;
    OperandDescriptor valueOperand;
    OperandDescriptor operand;
public:
    DataDeclarationCommand(sizeOfOpcode op, OperandDescriptor addrOp, OperandDescriptor valOp)
    : opcode(op), operand(addrOp), valueOperand(valOp) {}
    
    void execute(ControlUnit & cu) override = 0;    
    void compute(ControlUnit& cu) override;    
};










