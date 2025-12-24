#pragma once

#include "../command.hpp"


/*!
*@brief описатель унарной команды. Результат выполнения операции помещается в первый операнд.
*/
class UnaryCommand : public Command
{
public:
    sizeOfOpcode opcode;
    OperandDescriptor operand;

public:
    UnaryCommand(sizeOfOpcode op, const OperandDescriptor &opDesc) : opcode(op), operand(opDesc) {}
    void execute(ControlUnit &cu) override = 0;
    void compute(ControlUnit &cu) override;
};
