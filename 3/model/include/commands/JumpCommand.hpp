#include "command.hpp"
#include "../controleUnit.hpp"

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

struct JmpCommand : public JumpCommand
{
    JmpCommand(sizeOfOpcode opcode, const OperandDescriptor &operand) : JumpCommand(opcode, operand) {}
    void execute(ControlUnit &cu) override {}

    std::string getName() const override { return "JMP"; }
};
