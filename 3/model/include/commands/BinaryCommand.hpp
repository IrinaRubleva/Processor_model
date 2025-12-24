#include "command.hpp"
#include "../controleUnit.hpp"


/*!
*@brief Описатель бинарной команды содержит следующую информацию: поле метки (может отсутствовать) и 
        указателя на операцию; два описателя операндов. Результат выполнения операции помещается в первый операнд.
*/

class  BinaryCommand: public Command{
public:
    sizeOfOpcode Opcode;
    OperandDescriptor operand1;
    OperandDescriptor operand2;
public:
    BinaryCommand(sizeOfOpcode Opcode, OperandDescriptor opDesc1,  OperandDescriptor opDesc2)
    : Opcode(Opcode), operand1(opDesc1), operand2(opDesc2){}

    void execute(ControlUnit& cu) override = 0;
    void compute(ControlUnit& cu) override; 
    std::string getName() const override = 0;
};



struct AddCommand: public BinaryCommand{
public:
    AddCommand(sizeOfOpcode op, const OperandDescriptor& left, const OperandDescriptor& right)
    : BinaryCommand(op, left, right) {}

    void execute(ControlUnit& cu){
        uint64_t a = operand1.getRawValue(cu.getRefBus());
        uint64_t b = operand2.getRawValue(cu.getRefBus());
        operand1.setRawValue(cu.getRefBus(),(a+b));

    }

    std::string getName() const override { return "ADD"; }
};


struct SubCommand: public BinaryCommand{
public:
    SubCommand(sizeOfOpcode op, const OperandDescriptor& left, const OperandDescriptor& right)
    : BinaryCommand(op, left, right) {}

    void execute(ControlUnit & cu){
        uint64_t a = operand1.getRawValue(cu.getRefBus());
        uint64_t b = operand2.getRawValue(cu.getRefBus());
        operand1.setRawValue(cu.getRefBus(),(a-b));
    }

    std::string getName() const override { return "SUB"; }
};


struct MulCommand: public BinaryCommand{
public:
    MulCommand(sizeOfOpcode op, const OperandDescriptor& left, const OperandDescriptor& right)
    : BinaryCommand(op, left, right) {}

    void execute(ControlUnit & cu){
        uint64_t a = operand1.getRawValue(cu.getRefBus());
        uint64_t b = operand2.getRawValue(cu.getRefBus());
        operand1.setRawValue(cu.getRefBus(),(a*b));
    }

    std::string getName() const override { return "MUL"; }
};


struct DivCommand: public BinaryCommand{
public:
    DivCommand(sizeOfOpcode op, const OperandDescriptor& left, const OperandDescriptor& right)
    : BinaryCommand(op, left, right) {}

    void execute(ControlUnit & cu){
        uint64_t a = operand1.getRawValue(cu.getRefBus());
        uint64_t b = operand2.getRawValue(cu.getRefBus());
        if(b == 0){
            throw "division on zero! Shame on you";
        }
        operand1.setRawValue(cu.getRefBus(),(a/b));
    }

    std::string getName() const override { return "DIV"; }
};


struct CmpCommand: public BinaryCommand{
public:
    CmpCommand(sizeOfOpcode op, const OperandDescriptor& left, const OperandDescriptor& right)
    : BinaryCommand(op, left, right) {}

    void execute(ControlUnit & cu){
        uint64_t a = operand1.getRawValue(cu.getRefBus());
        uint64_t b = operand2.getRawValue(cu.getRefBus());
        operand1.setRawValue(cu.getRefBus(), (a==b));
    }

    std::string getName() const override { return "CMP"; }
};


struct MovCommand: public BinaryCommand{
public:
    MovCommand(sizeOfOpcode op, const OperandDescriptor& left, const OperandDescriptor& right)
    : BinaryCommand(op, left, right) {}

    void execute(ControlUnit & cu){
        uint64_t b = operand2.getRawValue(cu.getRefBus());
        operand1.setRawValue(cu.getRefBus(), b);
    }

    std::string getName() const override { return "MOV"; }
};      
           




