#pragma once
#include "_UnaryCommand.hpp"



/*!
*@brief Декрементирует команду. Результат выполнения операции помещается в первый операнд.
*/ 
struct DecCommand : public UnaryCommand
{
    DecCommand(sizeOfOpcode op, const OperandDescriptor &operand) : UnaryCommand(op, operand) {}

    void execute(ControlUnit &cu) override
    {
        uint64_t a = operand.getRawValue(cu.getRefBus());
        operand.setRawValue(cu.getRefBus(), (--a));
    }
    std::string getName() const override { return "DEC"; }
};