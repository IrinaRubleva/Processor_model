#pragma once

#include "_DataDeclaration.hpp"


/*!
*@brief Команда объявления данных(2 байта). Результат выполнения операции помещается в первый операнд.
*/
struct DdCommand : public DataDeclarationCommand
{
    DdCommand(sizeOfOpcode op, OperandDescriptor opDesc, OperandDescriptor offset)
        : DataDeclarationCommand(op, opDesc, offset) {}

    void execute(ControlUnit &cu) override {}

    std::string getName() const override { return "DD"; }
};