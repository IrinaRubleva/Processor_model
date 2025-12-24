#pragma once
#include <string>
#include <functional>
#include <map>
#include <memory>
#include "../operand.hpp"

class Bus;
class ControlUnit;

using sizeOfOpcode = uint8_t;



//оставить зависимость только от декодера - в нём хранятся opCode и commandName. ключ - имя, значение - опкод


class Command{   
public:
    virtual ~Command() = default;
    virtual void execute(ControlUnit& cu) = 0;
    virtual void compute(ControlUnit& cu)= 0;
    virtual std::string getName() const = 0;
};




class CommandFactory{
public:
    std::unordered_map<std::string, uint8_t> OpcodeMap;
    std::unordered_map<uint8_t, std::unique_ptr<Command>> OpcodeCommandMap;
    virtual bool isValidOpcode(uint8_t opcode) const = 0;

    ~CommandFactory() = default;
    // using UnaryCreator = std::function<std::unique_ptr<Command>(const OperandDescriptor&)>;
    // using BinaryCreator = std::function<std::unique_ptr<Command>(
    //     const OperandDescriptor&, const OperandDescriptor&)>;
    // using JumpCreator = std::function<std::unique_ptr<Command>(const std::string&)>;
    // using DataCreator = std::function<std::unique_ptr<Command>(
    //     const std::string&, uint64_t)>;


    virtual std::unique_ptr<Command> createUnary(sizeOfOpcode Opcode, const OperandDescriptor& operand) = 0;
    virtual std::unique_ptr<Command> createBinary(sizeOfOpcode Opcode, const OperandDescriptor& left, const OperandDescriptor& right) = 0;
    virtual std::unique_ptr<Command> createJump(sizeOfOpcode Opcode, const OperandDescriptor& operand) = 0;
    virtual std::unique_ptr<Command> createDataDeclaration(sizeOfOpcode Opcode, const OperandDescriptor& operand, uint64_t offset) = 0;
    virtual std::unique_ptr<Command> createThreadInit(sizeOfOpcode Opcode, const OperandDescriptor& operand) = 0;
    virtual std::unique_ptr<Command> createThreadEnd(sizeOfOpcode Opcode, const OperandDescriptor& operand) = 0;

};

class ConcreteCommandFactory: public CommandFactory{
public:
    Command* getPrototype(uint8_t opcode) {
        auto it = OpcodeCommandMap.find(opcode);
        if (it == OpcodeCommandMap.end())
            throw std::runtime_error("Unknown opcode");
        return it->second.get();
    }
    bool isValidOpcode(uint8_t opcode) const override {
        return OpcodeCommandMap.count(opcode) > 0;
    }

    ConcreteCommandFactory() = default;
    ~ConcreteCommandFactory() = default;

    std::unique_ptr<Command> createUnary(sizeOfOpcode Opcode, const OperandDescriptor& operand) override;
    std::unique_ptr<Command> createBinary(sizeOfOpcode Opcode, const OperandDescriptor& left, const OperandDescriptor& right) override;
    std::unique_ptr<Command> createJump(sizeOfOpcode Opcode, const OperandDescriptor& operand) override;
    std::unique_ptr<Command> createDataDeclaration(sizeOfOpcode Opcode, const OperandDescriptor& operand, uint64_t offset) override;
    std::unique_ptr<Command> createThreadInit(sizeOfOpcode Opcode, const OperandDescriptor& operand) override;
    std::unique_ptr<Command> createThreadEnd(sizeOfOpcode Opcode, const OperandDescriptor& operand) override;
};
