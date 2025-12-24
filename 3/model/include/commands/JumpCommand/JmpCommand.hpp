#pragma once

#include "_JumpCommand.hpp"


/*!
*@brief Команда перехода. Результат выполнения операции помещается в первый операнд.
*/
struct JmpCommand : public JumpCommand
{
    JmpCommand(sizeOfOpcode opcode, const OperandDescriptor &operand) : JumpCommand(opcode, operand) {}
    void execute(ControlUnit &cu) override {}

    std::string getName() const override { return "JMP"; }
};
