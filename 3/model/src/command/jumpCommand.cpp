#include "../include/commands/JumpCommand/_JumpCommand.hpp"
#include "../include/commands/command.hpp"
#include "../include/operand.hpp"
#include "../include/controleUnit.hpp"



#include <iostream>

void JumpCommand::compute(ControlUnit &cu)
{
    uint64_t addr = operand.getValue(); 
    cu.setPC(addr);
}