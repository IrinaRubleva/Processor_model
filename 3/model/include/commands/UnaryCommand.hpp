#include "command.hpp"
#include "../controleUnit.hpp"


class UnaryCommand : public Command {
public:
    sizeOfOpcode opcode;
    OperandDescriptor operand;
    
public:
    UnaryCommand(sizeOfOpcode op, const OperandDescriptor& opDesc): opcode(op), operand(opDesc) {}
    void execute(ControlUnit & cu) override = 0;
    void compute(ControlUnit& cu) override;
};


struct IncCommand : public UnaryCommand {
    IncCommand(sizeOfOpcode op, const OperandDescriptor& operand) : UnaryCommand(op, operand) {}
    
    void execute(ControlUnit & cu) override {
        uint64_t a = operand.getRawValue(cu.getRefBus());
        operand.setRawValue(cu.getRefBus(),(++a));
    }

    std::string getName() const override { return "INC"; }
};


struct DecCommand: public UnaryCommand{
    DecCommand(sizeOfOpcode op, const OperandDescriptor& operand): UnaryCommand(op, operand) {}
    
    void execute(ControlUnit & cu) override {
        uint64_t a = operand.getRawValue(cu.getRefBus());
        operand.setRawValue(cu.getRefBus(),(--a));
    }
    std::string getName() const override { return "DEC"; }
};

struct NegCommand: public UnaryCommand{
    NegCommand(sizeOfOpcode op, const OperandDescriptor& operand) : UnaryCommand(op, operand) {}

    void execute(ControlUnit & cu) override {
        operand.setRawValue(cu.getRefBus(),((uint64_t)(-(int64_t)operand.getRawValue(cu.getRefBus()))));
    }
    std::string getName() const override { return "NEG"; }
};

struct NotCommand: public UnaryCommand{
    NotCommand(sizeOfOpcode op, const OperandDescriptor& operand): UnaryCommand(op, operand) {}
    
    void execute(ControlUnit & cu) override {
        operand.setRawValue(cu.getRefBus(),(~operand.getRawValue(cu.getRefBus())));
    }
    std::string getName() const override { return "NOT"; }
};


