#include <gtest/gtest.h>
#include "../include/processor.hpp"
#include "../include/assembler.hpp"
#include "../include/loader.hpp"


TEST(Test, BasicTest){
    Processor cpu(4096, 16);
   
    Loader loader;
    loader.loadConfig("../../config.json", cpu);
    
    std::string file = readFileToString("../../program2.txt");

    Decoder& decoder = cpu.getControlUnit()->getRefDecoder();

    
    auto& factory = decoder.getFactory();
    
    auto prog = Assembler::assemble(file, decoder);

    cpu.loadProgram(prog.programBytes);
  
    cpu.run();
}


TEST(Command, BinaryCommand){
    OperandDescriptor op1, op2;
    
}

