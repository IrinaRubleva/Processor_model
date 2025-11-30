#pragma once
#include <vector>
#include "command.hpp"
#include <memory>
#include "device/memoryDevice.hpp"

class ProgramMemory: public MultiByteMemoryDevice{
private:
    std::vector<std::shared_ptr<Command>> program;
public:  
    void loadProgramm(std::vector<std::shared_ptr<Command>> p);
    std::shared_ptr<Command> fetch(size_t){
        
    }
    size_t size() {return program.size();};
};