#pragma once
#include "_DataDeclaration.hpp"


/*!
*@brief Команда объявления данных(4 байта). Результат выполнения операции помещается в первый операнд.
*/
struct DwCommand : public DataDeclarationCommand
{
    DwCommand(sizeOfOpcode op, OperandDescriptor opDesc, OperandDescriptor offset)
        : DataDeclarationCommand(op, opDesc, offset) {}

    void execute(ControlUnit &cu) override {}

    std::string getName() const override { return "DW"; }
};