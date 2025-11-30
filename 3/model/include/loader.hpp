#pragma once
#include <fstream>
#include <memory>
#include "processor.hpp"
#include "device/alu.hpp"
#include "device/memory.hpp"

class Loader {
public:
    static void loadConfig(const std::string& file, Processor& cpu) {
        std::ifstream f(file);
        if (!f) throw std::runtime_error("Config not found");

        nlohmann::json j;
        f >> j;

        uint64_t addrSpace = j["addressSpace"];
        cpu.initBus(addrSpace);

        for (auto& dev : j["devices"]) {
            std::string name = dev["name"];
            std::string type = dev["type"];
            uint64_t base   = dev["base"];
            uint64_t size   = dev["size"];

            std::unique_ptr<Device> device;

            if (type == "multi_memory") {
                uint8_t block = dev["block"];
                device = std::make_unique<MultiByteMemoryDevice>(base, size, block);
            }

            else if (type == "single_memory") {
                device = std::make_unique<SingleByteMemoryDevice>(base, size);
            }

            else if (type == "alu_sync") {
                uint32_t period = dev["period"];
                device = std::make_unique<ALUDevice_Synchron>(period);
            }

            // attach to CPU + register in CU
            cpu.attachDevice(base, size, name, std::move(device));
        }

        cpu.controlUnit().setPC(j["entryPoint"]);
    }
};
