#pragma once
#include "_DataDeclaration.hpp"



/*!
*@brief Команда объявления данных (1 байт). Результат выполнения операции помещается в первый операнд.
*/

struct DbCommand : public DataDeclarationCommand
{
    DbCommand(sizeOfOpcode op, OperandDescriptor opDesc, OperandDescriptor offset)
        : DataDeclarationCommand(op, opDesc, offset) {}

    void execute(ControlUnit &cu) override
    {
    }

    std::string getName() const override { return "DB"; }
};
