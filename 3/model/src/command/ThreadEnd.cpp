#include "../include/commands/ThreadEndCommand/_ThreadEnd.hpp"
#include "../include/controleUnit.hpp"
#include <iostream>

void ThreadEndCommand::compute(ControlUnit& cu) {
    uint64_t c = cu.getThreadCount();
    cu.setThreadCount(--c);
    cu.decrementThreadCount();
}