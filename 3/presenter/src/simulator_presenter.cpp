#include "../include/simulator_presenter.hpp"
#include <sstream>
#include <fstream>
#include <iostream>

SimulatorPresenter::SimulatorPresenter() 
    : processor(nullptr) {
        createProcessor(4096, 16);
}

void SimulatorPresenter::createProcessor(uint64_t busAddressSpace, size_t numRegisters) {
    try {
        processor = std::make_unique<Processor>(busAddressSpace, numRegisters);
        std::cout << "[Presenter] Processor created successfully" << std::endl;
        
        loader = std::make_unique<Loader>();
        std::cout << "[Presenter] Loader created" << std::endl;
        return;
    } catch (const std::exception& e) {
        std::cerr << "[Presenter] ERROR creating processor: " << e.what() << std::endl;
        processor.reset();
        loader.reset();
        return;
    }
}




bool SimulatorPresenter::loadConfig(const std::string& configPath) {
    if (!processor) {
        std::cout << "Процессор не создан!";
        return false;
    }
    
    try {
        loader->loadConfig(configPath, *processor);
        return true;
    } catch (const std::exception& e) {
        std::cout << ("Ошибка загрузки конфигурации: %1");
        return false;
    }
}



bool SimulatorPresenter::loadProgram(const std::string& programPath) {
    if (!processor) {
        std::cout << "Процессор не создан!";
        return false;
    }
    
    try {
        processor->reset();
        std::ifstream file(programPath);
        if (!file) {
            throw std::runtime_error("Проблема открытия файла");
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        Decoder& decoder = processor->getControlUnit()->getRefDecoder();
        auto prog = Assembler::assemble(content, decoder);
        
        processor->loadProgram(prog.programBytes);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка загрузки программы: " << e.what() << std::endl;
        return false;
    }
}

void SimulatorPresenter::run() {
    std::cout << "SimulatorViewModel::run() called";
        if (!processor) {
            std::cout << "ERROR: Processor is nullptr!";
            return;
        }
        processor->run();
        std::cout << "Run cycle executed";
    }


void SimulatorPresenter::resetProcessor() {
    if (processor) {
        processor->reset();
    }
}



std::string SimulatorPresenter::showMemoryState() const {
    if (!processor || !processor->isValid()) {
        return "Processor not created or not properly initialized";
    }
    
    std::stringstream ss;

    try {
        Bus* bus = processor->getBus();
        
        ss << "=== Memory State ===\n\n";
        
       
        ss << "--- GPR (General Purpose Registers) ---\n";
        try {
            uint64_t gprBase = bus->getDeviceAddress("GPR");
            ss << "GPR base address: " << gprBase << " (0x" 
               << std::hex << gprBase << std::dec << ")\n\n";
            
            const int num_registers = 16;
            const int bytes_in_register = 4;
            
            for (int reg = 0; reg < num_registers; ++reg) {
                uint32_t regValue = 0;
                for (int i = 0; i < bytes_in_register; ++i) {
                    uint64_t addr = gprBase + reg * bytes_in_register + i;
                    regValue |= (uint32_t)bus->read(addr) << (i * 8);
                }
                
                ss << "R" << std::setw(2) << reg << ": " 
                   << std::setw(10) << regValue 
                   << " (0x" << std::hex << std::setw(8) << std::setfill('0') 
                   << regValue << std::dec << ")\n";
            }
        } catch (const std::exception& e) {
            ss << "GPR not available: " << e.what() << "\n";
        }
        
        ss << "\n";
        
        ss << "--- DataMemory ---\n";
        try {
            uint64_t dataMemBase = bus->getDeviceAddress("DataMemory");
            ss << "DataMemory base address: " << dataMemBase 
               << " (0x" << std::hex << dataMemBase << std::dec << ")\n\n";
            
            const int bytes_to_show = 64;
            const int bytes_in_line = 16;
            
            ss << "First " << bytes_to_show << " bytes (decimal):\n";
            for (int i = 0; i < bytes_to_show; ++i) {
                if (i % bytes_in_line == 0) {
                    if (i > 0) ss << "\n";
                    ss << "Addr " << std::setw(4) << (dataMemBase + i) << ": ";
                }
                
                uint8_t value = bus->read(dataMemBase + i);
                ss << std::setw(3) << (int)value << " ";
            }
            
            ss << "\n\n";
            
            
            // ss << "First " << bytes_to_show << " bytes (hex):\n";
            // for (int i = 0; i < bytes_to_show; ++i) {
            //     if (i % bytes_in_line == 0) {
            //         if (i > 0) ss << "\n";
            //         ss << "0x" << std::hex << std::setw(4) << std::setfill('0')
            //            << (dataMemBase + i) << std::dec << ": ";
            //     }
                
            //     uint8_t value = bus->read(dataMemBase + i);
            //     ss << std::hex << std::setw(2) << std::setfill('0')
            //        << (int)value << " ";
            // }
            // ss << std::dec << "\n\n";
            
        } catch (const std::exception& e) {
            ss << "DataMemory not available: " << e.what() << "\n";
        }
        
       
        ss << "--- Additional Info ---\n";
        ss << "Program Counter (PC): " << processor->getControlUnit()->getPC() 
           << " (0x" << std::hex << processor->getControlUnit()->getPC() 
           << std::dec << ")\n";
        ss << "Current time: " << processor->getCurrentTime() << " cycles\n";
        ss << "Running: " << (processor->isRunning() ? "Yes" : "No") << "\n";
        ss << "Device count: " << processor->getDeviceCount() << "\n\n";
        
        ss << "=== End of Inspection ===\n";
        
    } catch (const std::exception& e) {
        ss << "Error during inspection: " << e.what() << "\n";
    }
    
    return ss.str();

}
