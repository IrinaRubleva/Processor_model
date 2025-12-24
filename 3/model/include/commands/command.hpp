#pragma once
#include <string>
#include <functional>
#include <map>
#include <memory>
#include "../operand.hpp"


class Bus;
class ControlUnit;

using sizeOfOpcode = uint8_t;


class Command{   
public:
    virtual ~Command() = default;
    virtual void execute(ControlUnit& cu) = 0;
    virtual void compute(ControlUnit& cu)= 0;
    virtual std::string getName() const = 0;
};


class CommandFactory {
public:
    using Creator = std::function<std::unique_ptr<Command>(sizeOfOpcode, const std::vector<OperandDescriptor>&, uint64_t)>;

    void registerOpcode(uint8_t opcode, std::string name, Creator creator){
        creators[opcode] = std::move(creator);
        OpcodeMap[opcode] = std::move(name);
    }

    bool isValidOpcode(uint8_t opcode) const {
        return creators.count(opcode) != 0;
    }

    std::unique_ptr<Command> createCommand(uint8_t opcode, const std::vector<OperandDescriptor>& ops, uint64_t pc){
        return creators.at(opcode)(opcode, ops, pc);
    }

    const std::string& getName(uint8_t opcode) const {
        return OpcodeMap.at(opcode);
    }


    void registerClass(const std::string& className, Creator creator) {
        registry[className] = std::move(creator);
    }

    Creator getClass(const std::string& className) const {
        auto it = registry.find(className);
        if (it == registry.end())
            throw std::runtime_error("Unknown command class: " + className);
        return it->second;
    }

    std::unordered_map<uint8_t, std::string>& getOpcodeMap(){ return OpcodeMap;}


private:
    std::unordered_map<uint8_t, Creator> creators; //
    std::unordered_map<uint8_t, std::string> OpcodeMap; //opcode -> name

    std::unordered_map<std::string, Creator> registry;  //реестр команд
};
