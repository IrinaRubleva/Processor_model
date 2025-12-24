#include "command.hpp"
#include "../controleUnit.hpp"


class DataDeclarationCommand: public Command{
protected:
    sizeOfOpcode opcode;
    uint64_t offset;
    OperandDescriptor operand;
public:
    DataDeclarationCommand(sizeOfOpcode op, OperandDescriptor opDesc, uint64_t offset)
    : opcode(op), operand(opDesc), offset(offset){}
    
    void execute(ControlUnit & cu) override = 0;    
    void compute(ControlUnit& cu) override;    
};


struct DwDataDeclarationCommand: public DataDeclarationCommand{
    DwDataDeclarationCommand(sizeOfOpcode op, OperandDescriptor opDesc, uint64_t offset)
    : DataDeclarationCommand(op, opDesc, offset) {}

    void execute(ControlUnit & cu) override{}

    std::string getName() const override { return "DW"; }
};

struct DdDataDeclarationCommand: public DataDeclarationCommand{
    DdDataDeclarationCommand(sizeOfOpcode op, OperandDescriptor opDesc, uint64_t offset)
    : DataDeclarationCommand(op, opDesc, offset) {}

    void execute(ControlUnit & cu) override{}
    
    std::string getName() const override { return "DD"; }
};

struct DqDataDeclarationCommand: public DataDeclarationCommand{
    DqDataDeclarationCommand(sizeOfOpcode op, OperandDescriptor opDesc, uint64_t offset)
    : DataDeclarationCommand(op, opDesc, offset) {}

    void execute(ControlUnit & cu) override{}

    std::string getName() const override { return "DQ"; }
};


struct DbDataDeclarationCommand: public DataDeclarationCommand{
    DbDataDeclarationCommand(sizeOfOpcode op, OperandDescriptor opDesc, uint64_t offset)
    : DataDeclarationCommand(op, opDesc, offset) {}

    void execute(ControlUnit & cu) override{

    }

    std::string getName() const override { return "DB"; }
};







