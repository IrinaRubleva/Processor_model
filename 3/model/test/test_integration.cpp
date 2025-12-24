#include <gtest/gtest.h>
#include "../include/processor.hpp"
#include "../include/loader.hpp"
#include "../include/assembler.hpp"
#include <fstream>

class BinaryOpsIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        
        std::ofstream config("test_binary_config.json");
        config << R"({
            "addressSpace": 4096,
            "devices": [
                {"name": "ProgramMemory", "type": "multi_memory", "base": 0, "size": 1024},
                {"name": "GPR", "type": "single_memory", "base": 1024, "size": 64},
                {"name": "ALU", "type": "alu_sync", "base": 1088, "size": 32, "period": 5},
                {"name": "DataMemory", "type": "multi_memory", "base": 1120, "size": 256}
            ],
            "opCode": [
                {"opcode": 1, "name": "MOV", "class": "MovCommand"},
                {"opcode": 2, "name": "ADD", "class": "AddCommand"},
                {"opcode": 3, "name": "CMP", "class": "CmpCommand"}
            ]
        })";
        config.close();

        
        processor = std::make_unique<Processor>(4096, 32);
        Loader loader;
        loader.loadConfig("test_binary_config.json", *processor);
        
    }

    void TearDown() override {
        std::remove("test_binary_config.json");
    }

    std::unique_ptr<Processor> processor;
};

TEST_F(BinaryOpsIntegrationTest, MovCommand_ExecutesAndCoversCode) {
    
    std::string programCode = "MOV R1, 42";
    auto& decoder = processor->getControlUnit()->getRefDecoder();
    auto assembled = Assembler::assemble(programCode, decoder);

   
    auto* progMem = dynamic_cast<ProgramMemory*>(
        processor->getBus()->getDevice<ReadableDevice>("ProgramMemory").value()
    );
    for (size_t i = 0; i < assembled.programBytes.size(); i++) {
        progMem->write(i, assembled.programBytes[i]);
    }

    
    std::thread runner([&]() {
        processor->run();
    });

    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    processor->getControlUnit()->stopAllThreads();
    if (runner.joinable()) runner.join();

    SUCCEED();
}