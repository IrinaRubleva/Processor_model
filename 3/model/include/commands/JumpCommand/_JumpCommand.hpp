#pragma once

#include "../command.hpp"


/*!
*@brief Описатель перехода. Результат выполнения операции помещается в первый операнд.
*/
class JumpCommand : public Command
{
protected:
    sizeOfOpcode opcode;
    OperandDescriptor operand;

public:
    JumpCommand(sizeOfOpcode op, const OperandDescriptor &operand) : opcode(op), operand(operand) {}
    void execute(ControlUnit &cu) override = 0;
    
    void compute(ControlUnit &cu) override;
};
