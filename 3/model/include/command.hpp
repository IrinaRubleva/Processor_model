#pragma once
#include <string>
#include "bus.hpp"
#include "operationType.hpp"
#include "decoder.hpp"
#include <functional>
#include <map>
#include <memory>
#include "device/alu.hpp"
#include "controlUnit.hpp"
#include "operand.hpp"





/**
 * @brief структура, нужная для опций для редактирования команды
 */
struct Request{
    bool ChangeCommand = false; ///< Остаётся ли тип команды прежним?
    OperationType newOperation; ///< @see OperationType
    
    bool ChangeOperand1 = false; ///< Остаётся ли первый операнд прежним?
    OperandDescriptor newOp1;

    bool ChangeOperand2 = false; ///< Остаётся ли второй операнд прежним?
    OperandDescriptor newOp2;

    bool ChangeAdress = false; ///< Остаётся ли адрес прежним?
    uint64_t newSize;
};


class Command{
private:
    OperationType type;      
public:
    virtual ~Command() = default;
    virtual void execute(Bus& bus, ControlUnit& cu) = 0;
    virtual void edit(const Request& req) = 0;
};



//назначенный - alu
class UnaryCommand: public Command{
private:
    OperationType type;
    OperandDescriptor operand;
public:
    UnaryCommand(OperationType OpType, OperandDescriptor opDesc): type(OpType), operand(opDesc){}
    
    void execute(Bus& bus, ControlUnit& cu) override;
    void edit(const Request& req) override{
        if(req.ChangeCommand){ type = req.newOperation;}
        if(req.ChangeOperand1) {operand = req.newOp1;}
    }
};



//назначенный - alu
class  BinaryCommand: public Command{
private:
    OperationType type;
    OperandDescriptor operand1;
    OperandDescriptor operand2;
public:
    BinaryCommand(OperationType OpType, OperandDescriptor opDesc1,  OperandDescriptor opDesc2): type(OpType), operand1(opDesc1), operand2(opDesc2){}

    void execute(Bus& bus, ControlUnit& cu) override;
    void edit(const Request& req) override{
        if(req.ChangeOperand1){operand1 = req.newOp1;}
        if(req.ChangeOperand2){operand1 = req.newOp2;}
        if(req.ChangeCommand){ type = req.newOperation;}
    }
};


//назначенный - pc в controle unit??
class  JumpCommand: public Command{
    OperationType type;
    OperandDescriptor operand; //for address to jump in bus
public:
    JumpCommand(OperationType OpType, OperandDescriptor opDesc): type(OpType), operand(opDesc){}
    void execute(Bus& bus, ControlUnit& cu) override;
    void edit(const Request& req) override{
        if(req.ChangeCommand) {type = req.newOperation;}
    }
};

//память данных - Loader + ProgramMemory
class DataDeclarationCommand: public Command{
    OperationType type;
    OperandDescriptor operand;
public:
    DataDeclarationCommand(OperationType OpType, OperandDescriptor opDesc): type(OpType), operand(opDesc){}
    void execute(Bus& bus, ControlUnit& cu) override;
    void edit(const Request& req) override{
        if(req.ChangeCommand) {type = req.newOperation;}
    }
};


//назначенный - console Device
class ThreadInitCommand: public Command{
    OperationType type;
public:
    ThreadInitCommand(OperationType OpType): type(OpType) {}
    void execute(Bus& bus, ControlUnit& cu) override;
    void edit(const Request& req) override{
        if(req.ChangeCommand) {type = req.newOperation;}
    }
};

//назначенный - console Device
class ThreadEndCommand: public Command{
    OperationType type;
public:
    ThreadEndCommand(OperationType OpType): type(OpType) {}
    void execute(Bus& bus, ControlUnit& cu) override;
    void edit(const Request& req) override{
        if(req.ChangeCommand) {type = req.newOperation;}
    }
};




class CommandFactory{
public:
    ~CommandFactory() = default;
    virtual std::unique_ptr<Command> createUnary(OperationType op, const OperandDescriptor& operand) = 0;
    virtual std::unique_ptr<Command> createBinary(OperationType op, const OperandDescriptor& left, const OperandDescriptor& right) = 0;
    virtual std::unique_ptr<Command> createJump(OperationType op, const OperandDescriptor& operand) = 0;
    virtual std::unique_ptr<Command> createDataDeclaration(OperationType op, const OperandDescriptor& operand) = 0;
    virtual std::unique_ptr<Command> createThreadInit(OperationType op) = 0;
    virtual std::unique_ptr<Command> createThreadEnd(OperationType op) = 0;
};

class ConcreteCommandFactory: public CommandFactory{
public:
    ConcreteCommandFactory() = default;

    std::unique_ptr<Command> createUnary(OperationType op, const OperandDescriptor& operand) override{
        return std::make_unique<UnaryCommand>(op, operand);
    }
    std::unique_ptr<Command> createBinary(OperationType op, const OperandDescriptor& left, const OperandDescriptor& right) override{
        return std::make_unique<BinaryCommand>(op, left, right);
    }
    std::unique_ptr<Command> createJump(OperationType op, const OperandDescriptor& operand) override{
        return std::make_unique<JumpCommand>(op, operand);
    }
    std::unique_ptr<Command> createDataDeclaration(OperationType op, const OperandDescriptor& operand) override{
        return std::make_unique<DataDeclarationCommand>(op, operand);
    }
    std::unique_ptr<Command> createThreadInit(OperationType op) override{
        return std::make_unique<ThreadInitCommand>(op);
    }
    std::unique_ptr<Command> createThreadEnd(OperationType op) override{
        return std::make_unique<ThreadEndCommand>(op);
    }

private:
    std::map<OperationType, std::function<uint64_t(uint64_t,uint64_t)>>instructionTable;
};


