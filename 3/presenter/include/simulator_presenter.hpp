#pragma once

#include "../model/include/processor.hpp"
#include "../../model/include/loader.hpp"
#include "../../model/include/assembler.hpp"

class SimulatorPresenter {
public:
    SimulatorPresenter();
    ~SimulatorPresenter() = default;
    
    void createProcessor(uint64_t busAddressSpace = 4096, size_t numRegisters = 16);
    
    bool loadConfig(const std::string& configPath);
    bool loadProgram(const std::string& programPath);
    
    std::string showMemoryState() const;

    void resetProcessor();
    
    Processor* getProcessor() const { return processor.get(); }

    void run();

    bool isProcessorCreated() const { return processor != nullptr; }

private:
    std::unique_ptr<Processor> processor;
    std::unique_ptr<Loader> loader;
};