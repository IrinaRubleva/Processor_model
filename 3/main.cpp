#include <iostream>
#include "model/include/processor.hpp"
#include "model/include/assembler.hpp"
#include "model/include/loader.hpp"
#include <string>


int main() {
    std::cout << "1";
    Processor cpu(4096, 16);
    std::cout << "2";
    Loader loader;
    loader.loadConfig("../config.json", cpu);
    std::cout << "3";
    std::string file = readFileToString("../program2.txt");
    std::cout << "4";
    Decoder& decoder = cpu.getControlUnit()->getRefDecoder();

    std::cout << "\n=== Checking decoder opcodes ===" << std::endl;
    auto& factory = decoder.getFactory();
    std::cout << "OpcodeMap size: " << factory.OpcodeMap.size() << std::endl;
    std::cout << "OpcodeCommandMap size: " << factory.OpcodeCommandMap.size() << std::endl;
    
    std::cout << "OpcodeMap contents:" << std::endl;
    for (const auto& [name, code] : factory.OpcodeMap) {
        std::cout << "  " << name << " -> " << int(code) << std::endl;
    }
    

    auto prog = Assembler::assemble(file, decoder);

    cpu.loadProgram(prog.programBytes);
    std::cout << "5";
    //cpu.getControlUnit()->loadLabel(prog.label);
    cpu.getProgramMemory()->printState();
   std::cout << "6";
    
    cpu.run();
    std::cout << "7";


    try {
        Bus* bus = cpu.getBus();
        
        std::cout << "=== Memory State ===\n\n";
        
       
        std::cout << "--- GPR (General Purpose Registers) ---\n";
        try {
            uint64_t gprBase = bus->getDeviceAddress("GPR");
            std::cout << "GPR base address: " << gprBase << " (0x" 
               << std::hex << gprBase << std::dec << ")\n\n";
            
            const int num_registers = 16;
            const int bytes_in_register = 4;
            
            for (int reg = 0; reg < num_registers; ++reg) {
                uint32_t regValue = 0;
                for (int i = 0; i < bytes_in_register; ++i) {
                    uint64_t addr = gprBase + reg * bytes_in_register + i;
                    regValue |= (uint32_t)bus->read(addr) << (i * 8);
                }
                
                std::cout << "R" << std::setw(2) << reg << ": " 
                   << std::setw(10) << regValue 
                   << " (0x" << std::hex << std::setw(8) << std::setfill('0') 
                   << regValue << std::dec << ")\n";
            }
        } catch (const std::exception& e) {
            std::cout << "GPR not available: " << e.what() << "\n";
        }
        
        std::cout << "\n";
        
        std::cout << "--- DataMemory ---\n";
        try {
            uint64_t dataMemBase = bus->getDeviceAddress("DataMemory");
            std::cout << "DataMemory base address: " << dataMemBase 
               << " (0x" << std::hex << dataMemBase << std::dec << ")\n\n";
            
            const int bytes_to_show = 64;
            const int bytes_in_line = 16;
            
            std::cout << "First " << bytes_to_show << " bytes (decimal):\n";
            for (int i = 0; i < bytes_to_show; ++i) {
                if (i % bytes_in_line == 0) {
                    if (i > 0) std::cout << "\n";
                    std::cout << "Addr " << std::setw(4) << (dataMemBase + i) << ": ";
                }
                
                uint8_t value = bus->read(dataMemBase + i);
                std::cout << std::setw(3) << (int)value << " ";
            }
            
            std::cout << "\n\n";
            
            
            // std::cout << "First " << bytes_to_show << " bytes (hex):\n";
            // for (int i = 0; i < bytes_to_show; ++i) {
            //     if (i % bytes_in_line == 0) {
            //         if (i > 0) std::cout << "\n";
            //         std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0')
            //            << (dataMemBase + i) << std::dec << ": ";
            //     }
                
            //     uint8_t value = bus->read(dataMemBase + i);
            //     std::cout << std::hex << std::setw(2) << std::setfill('0')
            //        << (int)value << " ";
            // }
            // std::cout << std::dec << "\n\n";
            
        } catch (const std::exception& e) {
            std::cout << "DataMemory not available: " << e.what() << "\n";
        }
        
       
        std::cout << "--- Additional Info ---\n";
        std::cout << "Program Counter (PC): " << cpu.getControlUnit()->getPC() 
           << " (0x" << std::hex << cpu.getControlUnit()->getPC() 
           << std::dec << ")\n";
        std::cout << "Current time: " << cpu.getCurrentTime() << " cycles\n";
        std::cout << "Running: " << (cpu.isRunning() ? "Yes" : "No") << "\n";
        std::cout << "Device count: " << cpu.getDeviceCount() << "\n\n";
        
        std::cout << "=== End of Inspection ===\n";
        
    } catch (const std::exception& e) {
        std::cout << "Error during inspection: " << e.what() << "\n";
    }


        Bus* bus = cpu.getBus();
    uint64_t gprBase = bus->getDeviceAddress("GPR");

    // Попробуем записать в R1 вручную
    std::cout << "\n=== Manual test of GPR ===" << std::endl;
    for (int i = 0; i < 4; i++) {
        uint8_t value = 0xAA + i;  // Тестовые значения
        bus->write(gprBase + 1*4 + i, value);  // Запись в R1
        std::cout << "Wrote 0x" << std::hex << (int)value 
                << " to addr 0x" << (gprBase + 1*4 + i) << std::dec << std::endl;
    }

    // Прочитаем R1
    uint32_t r1_value = 0;
    for (int i = 0; i < 4; i++) {
        uint8_t byte = bus->read(gprBase + 1*4 + i);
        r1_value |= (uint32_t)byte << (i * 8);
        std::cout << "Read from addr 0x" << std::hex << (gprBase + 1*4 + i)
                << ": 0x" << (int)byte << std::dec << std::endl;
    }
    std::cout << "R1 value: " << r1_value 
            << " (0x" << std::hex << r1_value << std::dec << ")" << std::endl;
}