#pragma once
#include <memory>
#include <unordered_map>
#include "commands/command.hpp"

#include <iostream>

//class Command;
class ProgramMemory;
//class ConcreteCommandFactory;

class Decoder
{
public:
    Decoder() {
        factory = std::make_unique<ConcreteCommandFactory>();
        std::cout << "[Decoder] Default constructor - factory created at " << factory.get() << std::endl;
    }
    Decoder(std::unique_ptr<CommandFactory> factory) : factory(std::move(factory)) {}
    std::unique_ptr<Command> decode(uint64_t &pc, ProgramMemory &mem);
    CommandFactory& getFactory() { return *factory;}
    // ConcreteCommandFactory* getFactory() { 
    //     return dynamic_cast<ConcreteCommandFactory*>(factory.get()); 
    // }
    
    // const ConcreteCommandFactory* getFactory() const { 
    //     return dynamic_cast<const ConcreteCommandFactory*>(factory.get()); 
    // }

private:
    std::unique_ptr<CommandFactory> factory;
};

