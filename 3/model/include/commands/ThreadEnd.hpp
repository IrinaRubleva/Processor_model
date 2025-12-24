#include "command.hpp"
#include "../controleUnit.hpp"


class ThreadEndCommand final: public Command{
    sizeOfOpcode Opcode;
    OperandDescriptor operand;
public:
    ThreadEndCommand(sizeOfOpcode Opcode, const OperandDescriptor &operand): Opcode(Opcode), operand(operand) {}
    void execute(ControlUnit & cu) override {}; //оставим до многопоточки
   
    void compute(ControlUnit& cu) override;    

    
    std::string getName() const override {return "THRE";};
};

