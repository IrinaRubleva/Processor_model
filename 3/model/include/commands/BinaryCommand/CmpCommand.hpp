#pragma once
#include "_BinaryCommand.hpp"


/*!
*@brief Команда сравнения Результат выполнения операции помещается в первый операнд.
*/


struct CmpCommand: public BinaryCommand{
public:
    CmpCommand(sizeOfOpcode op, const OperandDescriptor& left, const OperandDescriptor& right)
    : BinaryCommand(op, left, right) {}

    void execute(ControlUnit & cu){
        uint64_t a = operand1.getRawValue(cu.getRefBus());
        uint64_t b = operand2.getRawValue(cu.getRefBus());
        operand1.setRawValue(cu.getRefBus(), (a==b));
    }

    std::string getName() const override { return "CMP"; }
};