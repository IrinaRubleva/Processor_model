#pragma once

#include "../command.hpp"




/*!
*@brief Описатель начала потока
*/

class ControlUnir;


class ThreadInitCommand final: public Command{
    sizeOfOpcode Opcode;
    OperandDescriptor operand;
public:
     ThreadInitCommand(sizeOfOpcode Opcode, const OperandDescriptor& op)
        : Opcode(Opcode), operand(op) {}
        
    void execute(ControlUnit & cu) override{}
    
    void compute(ControlUnit& cu) override;   

    std::string getName() const override {return "THRI";};
};


