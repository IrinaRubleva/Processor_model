#pragma once
#include "_BinaryCommand.hpp"



/*!
*@brief Команда сложения. Результат выполнения операции помещается в первый операнд.
*/

struct AddCommand : public BinaryCommand
{
public:
    AddCommand(sizeOfOpcode op, const OperandDescriptor &left, const OperandDescriptor &right)
        : BinaryCommand(op, left, right) {}

    void execute(ControlUnit &cu)
    {
        uint64_t a = operand1.getRawValue(cu.getRefBus());
        uint64_t b = operand2.getRawValue(cu.getRefBus());
        operand1.setRawValue(cu.getRefBus(), (a + b));
    }

    std::string getName() const override { return "ADD"; }
};