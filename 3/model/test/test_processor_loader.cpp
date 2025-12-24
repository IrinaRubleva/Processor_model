#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include "processor.hpp"
#include "loader.hpp"
#include "assembler.hpp"
#include "commands/registerCommand.hpp"

static std::string writeTmp(const std::string& name, const std::string& txt) {
    auto p = std::filesystem::temp_directory_path() / name;
    std::ofstream f(p);
    f << txt;
    f.close();
    return p.string();
}

TEST(processor_cpp, Processor_Processor) {
    Processor cpu(0x1000, 8);
    EXPECT_NE(cpu.getBus(), nullptr);
    EXPECT_NE(cpu.getControlUnit(), nullptr);
    EXPECT_TRUE(cpu.getControlUnit()->regs().size() == 8);
}

TEST(processor_cpp, Processor_run) {
    Processor cpu(0x1000, 4);
    
    cpu.setProgramMemory(std::make_unique<ProgramMemory>(0));
    cpu.run();
    SUCCEED();
}

TEST(loader_cpp, Loader_loadConfig) {
    Processor cpu(0x1000, 4);

    Loader loader;
    EXPECT_THROW(loader.loadConfig("__no_such_config__.json", cpu), std::runtime_error);

    const char* json = R"json(
{
  "addressSpace": 4096,
  "devices": [
    { "name": "ProgramMemory", "type": "multi_memory", "base": 0, "size": 64 },
    { "name": "DataMemory",    "type": "multi_memory", "base": 256, "size": 64 },
    { "name": "GPR",           "type": "multi_memory", "base": 512, "size": 128 },
    { "name": "ALU",           "type": "alu_sync", "base": 768, "size": 64, "period": 1 }
  ],
  "entryPoint": 0,
  "opCode": [
    { "opcode": 1, "name": "ADD",  "class": "AddCommand" },
    { "opcode": 2, "name": "THRE", "class": "ThreadEndCommand" }
  ]
}
)json";

    auto cfg = writeTmp("cfg.json", json);
    EXPECT_NO_THROW(loader.loadConfig(cfg, cpu));

    const char* bad = R"json(
{ "devices": [ { "name":"X", "type":"unknown", "base": 1, "size": 1 } ] }
)json";
    auto cfgBad = writeTmp("cfg_bad.json", bad);
    EXPECT_THROW(loader.loadConfig(cfgBad, cpu), std::runtime_error);
}

TEST(assembler_cpp, readFileToString) {
    auto p = writeTmp("asm.txt", "HELLO\n");
    EXPECT_EQ(readFileToString(p), "HELLO\n");
    EXPECT_THROW(readFileToString("__no_such_asm__.txt"), std::runtime_error);
}

TEST(assembler_cpp, Assembler_assemble) {
    Decoder dec;
    auto& factory = dec.getFactory();
    registerAllCommands(factory);

    factory.registerOpcode(1, "DB",   factory.getClass("DbCommand"));
    factory.registerOpcode(2, "DD",   factory.getClass("DdCommand"));
    factory.registerOpcode(3, "ADD",  factory.getClass("AddCommand"));
    factory.registerOpcode(4, "INC",  factory.getClass("IncCommand"));
    factory.registerOpcode(5, "JMP",  factory.getClass("JmpCommand"));
    factory.registerOpcode(6, "THRI", factory.getClass("ThreadInitCommand"));
    factory.registerOpcode(7, "THRE", factory.getClass("ThreadEndCommand"));
    factory.registerOpcode(8, "MOV",  factory.getClass("MovCommand"));
    factory.registerOpcode(9, "CMP",  factory.getClass("CmpCommand"));

    Assembler a;

    const std::string ok = R"asm(
start:
DB x 7
DD y 100
MOV R1, 5
CMP R1, 5
ADD 10, 32
INC 1
THRI start
JMP start
THRE
)asm";
    auto out = a.assemble(ok, dec);
    EXPECT_GT(out.programBytes.size(), 0u);

    
    EXPECT_THROW(a.assemble("NOPE 1\n", dec), std::runtime_error);

    
    EXPECT_THROW(a.assemble("JMP a b\n", dec), std::runtime_error);

    
    EXPECT_THROW(a.assemble("JMP nowhere\n", dec), std::runtime_error);

    
    EXPECT_THROW(a.assemble("INC ???\n", dec), std::runtime_error);
}
