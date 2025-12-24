#pragma once
#include "../command.hpp"

/*!
*@brief Описатель бинарной команды содержит следующую информацию: поле метки (может отсутствовать) и
        указателя на операцию; два описателя операндов. Результат выполнения операции помещается в первый операнд.
*/

class BinaryCommand : public Command
{
public:
    sizeOfOpcode Opcode;
    OperandDescriptor operand1;
    OperandDescriptor operand2;

public:
    BinaryCommand(sizeOfOpcode Opcode, OperandDescriptor opDesc1, OperandDescriptor opDesc2)
        : Opcode(Opcode), operand1(opDesc1), operand2(opDesc2) {}

    void execute(ControlUnit &cu) override = 0;
    void compute(ControlUnit &cu) override;
    std::string getName() const override = 0;
};
