
#include <gtest/gtest.h>
#include "../include/assembler.hpp"
#include "../include/decoder.hpp"
#include "../include/commands/command.hpp"
#include "../include/operand.hpp"
#include <memory>
#include <string>
#include <vector>
#include <cctype>

#include <gtest/gtest.h>

#include "../include/device/memoryDevice.hpp" 

TEST(memoryDevice_cpp, SingleByteMemoryDevice_read)
{
    SingleByteMemoryDevice dev;
    dev.write(8, 0xAB);
    EXPECT_EQ(dev.read(8), 0xAB);

    EXPECT_EQ(dev.read(0), 0xFF);
    EXPECT_EQ(dev.read(7), 0xFF);
    EXPECT_EQ(dev.read(9), 0xFF);
}

TEST(memoryDevice_cpp, SingleByteMemoryDevice_write)
{
    SingleByteMemoryDevice dev;

    dev.write(8, 0x11);
    EXPECT_EQ(dev.read(8), 0x11);

    dev.write(0, 0x22);
    EXPECT_EQ(dev.read(8), 0x11);
}

TEST(memoryDevice_cpp, SingleByteMemoryDevice_ReadOnly_read)
{
    SingleByteMemoryDevice_ReadOnly dev;

    EXPECT_NE(dev.read(8), 0xFF);
    EXPECT_EQ(dev.read(0), 0xFF);
}

TEST(memoryDevice_cpp, MultiByteMemoryDevice_read)
{
    MultiByteMemoryDevice dev(4);

    dev.write(0, 0x10);
    dev.write(3, 0x33);

    EXPECT_EQ(dev.read(0), 0x10);
    EXPECT_EQ(dev.read(3), 0x33);

    // out of range -> 0xFF
    EXPECT_EQ(dev.read(4), 0xFF);
    EXPECT_EQ(dev.read(100), 0xFF);
}

TEST(memoryDevice_cpp, MultiByteMemoryDevice_write)
{
    MultiByteMemoryDevice dev(2);

    dev.write(0, 0xAA);
    dev.write(1, 0xBB);
    EXPECT_EQ(dev.read(0), 0xAA);
    EXPECT_EQ(dev.read(1), 0xBB);

    // out of range write: не падает и не портит данные
    dev.write(2, 0xCC);
    EXPECT_EQ(dev.read(0), 0xAA);
    EXPECT_EQ(dev.read(1), 0xBB);
}

TEST(memoryDevice_cpp, MultiByteMemoryDevice_ReadOnly_read)
{
    MultiByteMemoryDevice_ReadOnly dev(2);

    // ветка port < size
    EXPECT_NE(dev.read(0), 0xFF);
    EXPECT_NE(dev.read(1), 0xFF);

    // ветка иначе
    EXPECT_EQ(dev.read(2), 0xFF);
}

TEST(memoryDevice_cpp, RegisterBlockDevice_write)
{
    RegisterBlockDevice dev(4);

    dev.write(0, 0x01);
    dev.write(3, 0xFE);
    RegisterBlockDevice_ReadOnly ro(4);
    (void)ro;

    
    dev.write(1, 0x02);

    dev.write(4, 0x03);
    dev.write(999, 0x04);

    SUCCEED();
}

TEST(memoryDevice_cpp, RegisterBlockDevice_ReadOnly_read)
{
    RegisterBlockDevice_ReadOnly dev(2);

    EXPECT_NE(dev.read(0), 0xFF);
    EXPECT_NE(dev.read(1), 0xFF);

    EXPECT_EQ(dev.read(2), 0xFF);
}

class TestCommand : public Command
{
public:
    TestCommand(sizeOfOpcode op = 0, const std::vector<OperandDescriptor> &ops = {}, uint64_t pc = 0)
        : opcode(op) {}

    void execute(ControlUnit &cu) override {}
    void compute(ControlUnit &cu) override {}
    std::string getName() const override { return "Test"; }

private:
    sizeOfOpcode opcode;
};


class AssemblerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        decoder = std::make_unique<Decoder>();
        auto &factory = decoder->getFactory();

        
        auto createTestCommand = [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t pc)
        {
            return std::make_unique<TestCommand>(op, ops, pc);
        };

        
        factory.registerOpcode(0x01, "MOV", createTestCommand);
        factory.registerOpcode(0x02, "ADD", createTestCommand);
        factory.registerOpcode(0x03, "SUB", createTestCommand);
        factory.registerOpcode(0x04, "MUL", createTestCommand);
        factory.registerOpcode(0x05, "DIV", createTestCommand);
        factory.registerOpcode(0x06, "CMP", createTestCommand);
        factory.registerOpcode(0x07, "INC", createTestCommand);
        factory.registerOpcode(0x08, "DEC", createTestCommand);
        factory.registerOpcode(0x09, "NEG", createTestCommand);
        factory.registerOpcode(0x0A, "NOT", createTestCommand);

        
        factory.registerOpcode(0x10, "JMP", createTestCommand);
        factory.registerOpcode(0x11, "THRI", createTestCommand);
        factory.registerOpcode(0x12, "THRE", createTestCommand);

        
        factory.registerOpcode(0x20, "DB", createTestCommand);
        factory.registerOpcode(0x21, "DW", createTestCommand);
        factory.registerOpcode(0x22, "DD", createTestCommand);
        factory.registerOpcode(0x23, "DQ", createTestCommand);
    }

    std::unique_ptr<Decoder> decoder;

    static bool isRegisterHelper(const std::string &s)
    {
        if (s.size() < 2 || s[0] != 'R')
            return false;
        if (!std::isdigit(s[1]))
            return false;

        for (size_t i = 2; i < s.size(); i++)
        {
            if (!std::isdigit(s[i]))
                return false;
        }
        return true;
    }
};

TEST_F(AssemblerTest, BasicAssembly)
{
    std::string code =
        "MOV R1, 42\n"
        "ADD R2, R1\n"
        "INC R3\n";

    auto result = Assembler::assemble(code, *decoder);
    EXPECT_FALSE(result.programBytes.empty());
}

TEST_F(AssemblerTest, DataDeclaration)
{
    std::string code =
        "DB var1 255\n"
        "DW var2 65535\n"
        "DD var3 4294967295\n"
        "DQ var4 18446744073709551615\n";

    auto result = Assembler::assemble(code, *decoder);
    EXPECT_EQ(result.dataVariables.size(), 4);
    EXPECT_EQ(result.dataVariables["var1"], 0);
    EXPECT_EQ(result.dataVariables["var2"], 1); 
    EXPECT_EQ(result.dataVariables["var3"], 3); 
    EXPECT_EQ(result.dataVariables["var4"], 7); 
}

TEST_F(AssemblerTest, LabelsAndJumps)
{
    std::string code =
        "MOV R1, 10\n"
        "start:\n"
        "INC R1\n"
        "JMP start\n";

    auto result = Assembler::assemble(code, *decoder);
    EXPECT_EQ(result.label.size(), 1);
    EXPECT_TRUE(result.label.find("start") != result.label.end());
}

TEST_F(AssemblerTest, ThreadCommands)
{
    std::string code =
        "THRE\n"
        "label:\n"
        "THRI label\n";

    auto result = Assembler::assemble(code, *decoder);
    
    EXPECT_GE(result.ThreadCount, 1);
}

TEST_F(AssemblerTest, InvalidOpcodeThrows)
{
    std::string code = "INVALID R1, R2\n";

    EXPECT_THROW(
        Assembler::assemble(code, *decoder),
        std::runtime_error);
}

TEST_F(AssemblerTest, RegisterParsing)
{
    std::string code = "MOV R1, R2\n";

    auto result = Assembler::assemble(code, *decoder);
    EXPECT_FALSE(result.programBytes.empty());
}

TEST_F(AssemblerTest, ImmediateValues)
{
    std::string code =
        "MOV R1, 1234\n"
        "ADD R2, -567\n";

    auto result = Assembler::assemble(code, *decoder);
    EXPECT_FALSE(result.programBytes.empty());
}

TEST_F(AssemblerTest, EmptyLinesAndComments)
{
    std::string code =
        "\n"
        "MOV R1, 10\n"
        "\n"
        "INC R1\n"
        "\n";

    auto result = Assembler::assemble(code, *decoder);
    EXPECT_FALSE(result.programBytes.empty());
}

TEST_F(AssemblerTest, TrimFunction)
{
    auto trim = [](const std::string &s)
    {
        size_t b = s.find_first_not_of(" \t");
        size_t e = s.find_last_not_of(" \t");
        if (b == std::string::npos)
            return std::string("");
        return s.substr(b, e - b + 1);
    };

    EXPECT_EQ(trim("  hello  "), "hello");
    EXPECT_EQ(trim("\thello\t"), "hello");
    EXPECT_EQ(trim(""), "");
    EXPECT_EQ(trim("   "), "");
}

TEST_F(AssemblerTest, IsRegisterFunction)
{
    EXPECT_TRUE(isRegisterHelper("R0"));
    EXPECT_TRUE(isRegisterHelper("R1"));
    EXPECT_TRUE(isRegisterHelper("R10"));
    EXPECT_TRUE(isRegisterHelper("R255"));
    EXPECT_FALSE(isRegisterHelper("r1"));   
    EXPECT_FALSE(isRegisterHelper("X1"));   
    EXPECT_FALSE(isRegisterHelper("R"));    
    EXPECT_FALSE(isRegisterHelper("R1a"));  
    EXPECT_FALSE(isRegisterHelper("Ra1"));  
    EXPECT_FALSE(isRegisterHelper("R1a2")); 
}

TEST_F(AssemblerTest, UnaryCommands)
{
    std::string code =
        "INC R1\n"
        "DEC R2\n"
        "NEG R3\n"
        "NOT R4\n";

    auto result = Assembler::assemble(code, *decoder);
    EXPECT_FALSE(result.programBytes.empty());
}

TEST_F(AssemblerTest, BinaryCommands)
{
    std::string code =
        "MOV R1, R2\n"
        "ADD R3, R4\n"
        "SUB R5, R6\n"
        "MUL R7, R8\n"
        "DIV R9, R10\n"
        "CMP R11, R12\n";

    auto result = Assembler::assemble(code, *decoder);
    EXPECT_FALSE(result.programBytes.empty());
}