#pragma once

#include "../command.hpp"


/*!
*@brief Описатель конца потока
*/

class ControlUnir;

class ThreadEndCommand final: public Command{
    sizeOfOpcode Opcode;
    OperandDescriptor operand;
public:
    ThreadEndCommand(sizeOfOpcode Opcode, const OperandDescriptor& op)
        : Opcode(Opcode), operand(op) {}
        
    void execute(ControlUnit & cu) override{}
   
    void compute(ControlUnit& cu) override;    

    
    std::string getName() const override {return "THRE";};
};

