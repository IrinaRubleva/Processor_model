#pragma once
#include <memory>
#include <unordered_map>
#include "commands/command.hpp"

#include <iostream>




/*!
*@brief Декодер. Достаёт команду из ProgramMemory и возвращает тип команды
*       Содержит factory, в котором есть таблицы опкодов и названий команд
*/

class ProgramMemory;
class CommandFactory;

class Decoder
{
public:
    Decoder() : factory(std::make_unique<CommandFactory>()) {}
    
    explicit Decoder(std::unique_ptr<CommandFactory> factory): factory(std::move(factory)) {}

    std::unique_ptr<Command> decode(uint64_t& pc, ProgramMemory& mem);

    CommandFactory& getFactory() { return *factory; }
    CommandFactory& getFactory() const { return *factory; }

     void setFactory(std::unique_ptr<CommandFactory> f) {
        factory = std::move(f);
    }
private:
    std::unique_ptr<CommandFactory> factory;
};

