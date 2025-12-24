#pragma once
#include "_UnaryCommand.hpp"


/*!
*@brief Команда смены знака. Результат выполнения операции помещается в первый операнд.
*/ 

struct NegCommand: public UnaryCommand{
    NegCommand(sizeOfOpcode op, const OperandDescriptor& operand) : UnaryCommand(op, operand) {}

    void execute(ControlUnit & cu) override {
        operand.setRawValue(cu.getRefBus(),((uint64_t)(-(int64_t)operand.getRawValue(cu.getRefBus()))));
    }
    std::string getName() const override { return "NEG"; }
};