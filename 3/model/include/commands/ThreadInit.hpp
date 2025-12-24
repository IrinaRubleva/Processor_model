#include "command.hpp"
#include "../controleUnit.hpp"


class ThreadInitCommand final: public Command{
    sizeOfOpcode Opcode;
    OperandDescriptor operand;
public:
    ThreadInitCommand(sizeOfOpcode Opcode, const OperandDescriptor &operand): Opcode(Opcode), operand(operand) {}
    void execute(ControlUnit & cu) override {}; //оставим до многопоточки
    
    void compute(ControlUnit& cu) override;   

    std::string getName() const override {return "THRI";};
};


