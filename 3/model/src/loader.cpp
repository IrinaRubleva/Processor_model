#include <nlohmann/json.hpp>
#include <fstream>
#include "../include/loader.hpp"
#include "../include/commands/command.hpp"
#include "../include/commands/registerCommand.hpp"
//#include "../include/commands/_plugins/PluginLoader.hpp"


void Loader::loadConfig(const std::string &file, Processor &cpu)
{
    std::ifstream f(file);
    if (!f) throw std::runtime_error("Config file not found");

    nlohmann::json j;
    f >> j;

    if (j.contains("addressSpace"))
        cpu.getBus()->setAddressSpaceSize(j["addressSpace"]);

    // if (j.contains("controleUnitPeriod"))
    //     cpu.getControlUnit()->setPeriod(j["controleUnitPeriod"]);
    uint64_t maxThreads = 0;
    if (j.contains("maxThreads")) {
        maxThreads = j["maxThreads"];
        //cpu.getControlUnit()->getThreadManager()->setMaxThreads(maxThreads);
    }

    if (j.contains("devices")) {
        for (auto &dev : j["devices"]) {
            std::string name = dev["name"];
            std::string type = dev["type"];
            uint64_t base = dev["base"];
            uint64_t size = dev["size"];

            std::unique_ptr<ReadableDevice> device;

            if (type == "multi_memory") {
                if (name == "ProgramMemory") {
                    auto progMem = std::make_unique<ProgramMemory>(size);
                    cpu.getBus()->attachDevice(name, base, size, progMem.get());
                    cpu.setProgramMemory(std::move(progMem));
                    continue;
                } else {
                    device = std::make_unique<MultiByteMemoryDevice>(size);
                }
            } else if (type == "single_memory") {
                device = std::make_unique<SingleByteMemoryDevice>(size);
            } else if (type == "alu_sync") {
                uint64_t period = dev["period"];
                device = std::make_unique<ALUDevice_Synchron>(period);
            } else if (type == "alu_async") {
                device = std::make_unique<ALUDevice_Asynchron>();
                
            } else {
                throw std::runtime_error("Unknown device type: " + type);
            }

            cpu.attachDevice(name, base, size, std::move(device));
        }
    }

    if (j.contains("entryPoint"))
        cpu.getControlUnit()->setPC(j["entryPoint"]);


    Decoder& decoder = cpu.getControlUnit()->getRefDecoder();
    CommandFactory& factory = decoder.getFactory();
    
    
    registerAllCommands(factory);
    //PluginLoader loader(factory);
    //loader.loadAllFromDir("../include/commands/_plugins");

    if (j.contains("opCode")) {
        Decoder& decoder = cpu.getControlUnit()->getRefDecoder();
        CommandFactory& factory = decoder.getFactory();

        for (auto& entry : j["opCode"]) {
            uint8_t opcode        = entry["opcode"];
            std::string name      = entry["name"];
            std::string className = entry["class"];

            auto creator = factory.getClass(className);

            factory.registerOpcode(opcode, name, creator);
        }
    }
}
