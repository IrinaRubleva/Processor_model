#include <iostream>
#include "model/include/processor.hpp"
#include "model/include/assembler.hpp"
#include "model/include/loader.hpp"
#include <string>

#include <chrono>


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

    using clock_t = std::chrono::high_resolution_clock;
    auto start = clock_t::now();
    cpu.run();
    
    auto end = clock_t::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Execution time: " 
            << duration.count() 
            << " us" << std::endl;

    std::cout << "Cycles: " << cpu.getCurrentTime() << std::endl;
}