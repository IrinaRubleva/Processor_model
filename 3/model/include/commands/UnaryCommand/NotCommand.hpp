#pragma once
#include "_UnaryCommand.hpp"



/*!
*@brief Команда отрицания. Результат выполнения операции помещается в первый операнд.
*/ 

struct NotCommand : public UnaryCommand
{
    NotCommand(sizeOfOpcode op, const OperandDescriptor &operand) : UnaryCommand(op, operand) {}

    void execute(ControlUnit &cu) override
    {
        operand.setRawValue(cu.getRefBus(), (~operand.getRawValue(cu.getRefBus())));
    }
    std::string getName() const override { return "NOT"; }
};
