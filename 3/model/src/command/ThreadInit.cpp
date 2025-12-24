#include "../include/commands/ThreadInitCommand/_ThreadInit.hpp"
#include "../include/controleUnit.hpp"
#include <iostream>


void ThreadInitCommand::compute(ControlUnit& cu) {
    uint64_t addr = operand.getValue(); 
    cu.createNewThread(addr);
}