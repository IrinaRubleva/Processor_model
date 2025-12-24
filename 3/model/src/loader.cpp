#include <nlohmann/json.hpp>
#include <fstream>
#include <memory>
#include "loader.hpp"
#include "../include/commands/BinaryCommand.hpp"
#include "../include/commands/UnaryCommand.hpp"
#include "../include/commands/JumpCommand.hpp"
#include "../include/commands/DataDeclaration.hpp"
#include "../include/commands/ThreadInit.hpp"
#include "../include/commands/ThreadEnd.hpp"

typedef uint8_t sizeOfOpcode;

std::unique_ptr<Command> createCommandPrototype(const std::string &className, sizeOfOpcode opcode, const std::string &type)
{
    if (type == "unary")
    {
        OperandDescriptor dummy(TypeData::Data, 4, 0);
        if (className == "IncCommand")
            return std::make_unique<IncCommand>(opcode, dummy);
        if (className == "DecCommand")
            return std::make_unique<DecCommand>(opcode, dummy);
        if (className == "NegCommand")
            return std::make_unique<NegCommand>(opcode, dummy);
        if (className == "NotCommand")
            return std::make_unique<NotCommand>(opcode, dummy);
    }
    else if (type == "binary")
    {
        OperandDescriptor dummy1(TypeData::Data, 4, 0);
        OperandDescriptor dummy2(TypeData::Data, 4, 0);
        if (className == "AddCommand")
            return std::make_unique<AddCommand>(opcode, dummy1, dummy2);
        if (className == "SubCommand")
            return std::make_unique<SubCommand>(opcode, dummy1, dummy2);
        if (className == "MulCommand")
            return std::make_unique<MulCommand>(opcode, dummy1, dummy2);
        if (className == "DivCommand")
            return std::make_unique<DivCommand>(opcode, dummy1, dummy2);
        if (className == "CmpCommand")
            return std::make_unique<CmpCommand>(opcode, dummy1, dummy2);
        if (className == "MovCommand")
            return std::make_unique<MovCommand>(opcode, dummy1, dummy2);
    }
    else if (type == "jump")
    {
        OperandDescriptor dummy(TypeData::Address, 8, 0);

        if (className == "JmpCommand")
            return std::make_unique<JmpCommand>(opcode, dummy);

        throw std::runtime_error("Unknown jump command class: " + className);
    }

    else if (type == "dataDeclaraid")
    { 
        OperandDescriptor dummy(TypeData::Data, 4, 0);
        uint64_t dummyOffset = 0;

        if (className == "DbCommand" || className == "DbDataDeclarationCommand")
            return std::make_unique<DbDataDeclarationCommand>(opcode, dummy,dummyOffset);

        if (className == "DwCommand" || className == "DwDataDeclarationCommand")
            return std::make_unique<DwDataDeclarationCommand>(opcode, dummy,dummyOffset);

        if (className == "DdCommand" || className == "DdDataDeclarationCommand")
            return std::make_unique<DdDataDeclarationCommand>(opcode, dummy,dummyOffset);

        if (className == "DqCommand" || className == "DqDataDeclarationCommand")
            return std::make_unique<DqDataDeclarationCommand>(opcode, dummy,dummyOffset);

        throw std::runtime_error("Unknown data declaration command class: " + className);
    }

    else if (type == "threadInit")
    {
        OperandDescriptor dummy(TypeData::Address, 8, 0);
        if (className == "ThreadInitCommand")
            return std::make_unique<ThreadInitCommand>(opcode, dummy);
        throw std::runtime_error("Unknown thread init command class: " + className);
    }

    else if (type == "threadEnd")
    {
        OperandDescriptor dummy(TypeData::Address, 8, 0);
        if (className == "ThreadEndCommand")
            return std::make_unique<ThreadEndCommand>(opcode, dummy);

        throw std::runtime_error("Unknown thread end command class: " + className);
    }

    else
    {
        throw std::runtime_error("Unknown command type: " + type);
    }

    return nullptr;
}

void Loader::loadConfig(const std::string &file, Processor &cpu)
{
    std::ifstream f(file);
    if (!f)
        throw std::runtime_error("Config not found");

    nlohmann::json j;
    f >> j;

    if (j.contains("addressSpace"))
    {
        cpu.getBus()->setAddressSpaceSize(j["addressSpace"]);
    }

    if (j.contains("controleUnitPeriod"))
    {
        cpu.getControlUnit()->setPeriod(j["controleUnitPeriod"]);
    }

    if (j.contains("devices"))
    {
        for (auto &dev : j["devices"])
        {
            std::string name = dev["name"];
            std::string type = dev["type"];
            uint64_t base = dev["base"];
            uint64_t size = dev["size"];

            std::unique_ptr<ReadableDevice> device;

            if (type == "multi_memory")
            {
                if (name == "ProgramMemory")
                {

                    auto newProgMem = std::make_unique<ProgramMemory>(size);
                    cpu.getBus()->attachDevice(name, base, size, newProgMem.get());
                    cpu.setProgramMemory(std::move(newProgMem));
                    continue;
                }
                else
                {
                    device = std::make_unique<MultiByteMemoryDevice>(size);
                }
            }
            else if (type == "single_memory")
            {
                device = std::make_unique<SingleByteMemoryDevice>(size);
            }
            else if (type == "alu_sync")
            {
                uint64_t period = dev["period"];
                device = std::make_unique<ALUDevice_Synchron>(period);
            }
            else if (type == "alu_async")
            {
                device = std::make_unique<ALUDevice_Asynchron>();
            }
            else
            {
                throw std::runtime_error("Unknown device type: " + type);
            }

            if (device)
            {
                cpu.attachDevice(name, base, size, std::move(device));
            }
        }
    }

    if (j.contains("entryPoint"))
    {
        cpu.getControlUnit()->setPC(j["entryPoint"]);
    }
    if (j.contains("opCode"))
    {
        Decoder *decoder = cpu.getControlUnit()->getDecoder();
        if (!decoder)
        {
            throw std::runtime_error("Decoder is null");
        }

        CommandFactory *abstractFactory = &decoder->getFactory();

        ConcreteCommandFactory *factory = dynamic_cast<ConcreteCommandFactory *>(abstractFactory);
        if (!factory)
        {
            throw std::runtime_error("Factory is not ConcreteCommandFactory");
        }

        if (j.contains("opCode"))
        {
            Decoder *decoder = cpu.getControlUnit()->getDecoder();
            ConcreteCommandFactory *factory = dynamic_cast<ConcreteCommandFactory *>(&decoder->getFactory());

            for (auto &opcodeEntry : j["opCode"])
            {
                std::string name = opcodeEntry["name"];
                sizeOfOpcode opcode = opcodeEntry["opcode"];
                std::string type = opcodeEntry["type"];
                std::string className = opcodeEntry["class"];

                factory->OpcodeMap[name] = opcode;

                std::unique_ptr<Command> prototype = createCommandPrototype(className, opcode, type);
                if (prototype)
                {
                    factory->OpcodeCommandMap[opcode] = std::move(prototype);
                }
            }
        }
    }
}