#pragma once
#include "_DataDeclaration.hpp"


/*!
*@brief Команда объявления данных(8 байт). Результат выполнения операции помещается в первый операнд.
*/
struct DqCommand : public DataDeclarationCommand
{
    DqCommand(sizeOfOpcode op, OperandDescriptor opDesc, OperandDescriptor offset)
        : DataDeclarationCommand(op, opDesc, offset) {}

    void execute(ControlUnit &cu) override {}

    std::string getName() const override { return "DQ"; }
};