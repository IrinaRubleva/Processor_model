#include <iostream>
#include "model/include/processor.hpp"
#include "model/include/assembler.hpp"
#include "model/include/loader.hpp"
#include <string>


int main() {
    Processor cpu(4096, 16);
    
    Loader loader;
    loader.loadConfig("../config.json", cpu);
    
    std::string file = readFileToString("../program2.txt");
    
    Decoder& decoder = cpu.getControlUnit()->getRefDecoder();
    auto& factory = decoder.getFactory();    

    auto prog = Assembler::assemble(file, decoder);

    cpu.loadProgram(prog.programBytes);
    cpu.setThreadCount(prog.ThreadCount);

    cpu.run();    
}