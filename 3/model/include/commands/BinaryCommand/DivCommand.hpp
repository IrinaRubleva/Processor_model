#pragma once

#include "_BinaryCommand.hpp"


/*!
*@brief Команда деление. Результат выполнения операции помещается в первый операнд.
*/

struct DivCommand : public BinaryCommand
{
public:
    DivCommand(sizeOfOpcode op, const OperandDescriptor &left, const OperandDescriptor &right)
        : BinaryCommand(op, left, right) {}

    void execute(ControlUnit &cu)
    {
        uint64_t a = operand1.getRawValue(cu.getRefBus());
        uint64_t b = operand2.getRawValue(cu.getRefBus());
        if (b == 0)
        {
            throw std::runtime_error("division by zero");
        }
        operand1.setRawValue(cu.getRefBus(), (a / b));
    }

    std::string getName() const override { return "DIV"; }
};
