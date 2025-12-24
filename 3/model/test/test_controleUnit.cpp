#include <gtest/gtest.h>

#include "controleUnit.hpp"
#include "device/bus.hpp"
#include "device/memoryDevice.hpp"
#include "decoder.hpp"
#include "commands/command.hpp"

#include "device/bus.hpp"
#include "device/memoryDevice.hpp"
#include "controleUnit.hpp"
#include "operand.hpp"

#include "commands/BinaryCommand/CmpCommand.hpp"

#include <stdexcept>

#include "device/bus.hpp"
#include "controleUnit.hpp"
#include "operand.hpp"

#include "commands/BinaryCommand/DivCommand.hpp"

#include <sstream>
#include <iostream>

#include "device/bus.hpp"
#include "device/memoryDevice.hpp"
#include "controleUnit.hpp"
#include "operand.hpp"

#include "commands/BinaryCommand/MovCommand.hpp"
#include "commands/BinaryCommand/MulCommand.hpp"
#include "commands/BinaryCommand/DivCommand.hpp"
#include "commands/DataDeclaration/DbCommand.hpp"

#include <gtest/gtest.h>

#include "device/bus.hpp"
#include "controleUnit.hpp"
#include "operand.hpp"

#include "commands/DataDeclaration/DdCommand.hpp"
#include "commands/DataDeclaration/DqCommand.hpp"
#include "commands/DataDeclaration/DwCommand.hpp"

TEST(Command, DdCommand_ctor_execute_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DdCommand dd(
        1,
        OperandDescriptor(TypeData::Data, 4, 0x11223344),
        OperandDescriptor(TypeData::Data, 8, 0));

    dd.execute(cu);                
    EXPECT_EQ(dd.getName(), "DD"); 
}

TEST(Command, DqCommand_ctor_execute_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DqCommand dq(
        1,
        OperandDescriptor(TypeData::Data, 8, 0x1122334455667788ULL),
        OperandDescriptor(TypeData::Data, 8, 0));

    dq.execute(cu);
    EXPECT_EQ(dq.getName(), "DQ");
}

TEST(Command, DwCommand_ctor_execute_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DwCommand dw(
        1,
        OperandDescriptor(TypeData::Data, 2, 0xBEEF),
        OperandDescriptor(TypeData::Data, 8, 0));

    dw.execute(cu);
    EXPECT_EQ(dw.getName(), "DW");
}

TEST(Command, MovCommand_execute_DataData_coversCout)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    std::ostringstream capture;
    auto *old = std::cout.rdbuf(capture.rdbuf());

    MovCommand mov(
        1,
        OperandDescriptor(TypeData::Data, 8, 0),
        OperandDescriptor(TypeData::Data, 8, 123));
    mov.execute(cu);

    std::cout.rdbuf(old);

    EXPECT_EQ(mov.operand1.getValue(), 123u);
    EXPECT_NE(capture.str().find("Reading value"), std::string::npos);
}

TEST(Command, MovCommand_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    MovCommand mov(
        1,
        OperandDescriptor(TypeData::Data, 8, 0),
        OperandDescriptor(TypeData::Data, 8, 1));
    EXPECT_EQ(mov.getName(), "MOV");
}

// ---------- DIV ----------
TEST(Commdand, DivCommand_exdecute_ok)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DivCommand div(
        1,
        OperandDescriptor(TypeData::Data, 8, 84),
        OperandDescriptor(TypeData::Data, 8, 2));

    div.execute(cu);
    EXPECT_EQ(div.operand1.getValue(), 42u);
}

TEST(Command, DivCommand_execute_div0)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DivCommand div(
        1,
        OperandDescriptor(TypeData::Data, 8, 84),
        OperandDescriptor(TypeData::Data, 8, 0));

    EXPECT_THROW(div.execute(cu), std::runtime_error);
}

TEST(Command, DivCommcand_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DivCommand div(
        1,
        OperandDescriptor(TypeData::Data, 8, 1),
        OperandDescriptor(TypeData::Data, 8, 1));
    EXPECT_EQ(div.getName(), "DIV");
}

// ---------- MUL ----------
TEST(Command, MulCommand_execute_DataData)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    MulCommand mul(
        1,
        OperandDescriptor(TypeData::Data, 8, 6),
        OperandDescriptor(TypeData::Data, 8, 7));

    mul.execute(cu);
    EXPECT_EQ(mul.operand1.getValue(), 42u);
}

TEST(Command, MulCommand_execute_RegisterData)
{
    Bus bus(0x2000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice gpr(256);
    bus.attachDevice("GPR", 0x100, 256, &gpr);

    MulCommand mul(
        1,
        OperandDescriptor(TypeData::Register, 8, 1),
        OperandDescriptor(TypeData::Data, 8, 7));

    mul.execute(cu);

    EXPECT_EQ(42u, 42u);
}

TEST(Command, MulCommand_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    MulCommand mul(
        1,
        OperandDescriptor(TypeData::Data, 8, 1),
        OperandDescriptor(TypeData::Data, 8, 1));
    EXPECT_EQ(mul.getName(), "MUL");
}

// ---------- DB ----------
TEST(Command, DbCommand_ctor_execute_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    
    DbCommand db(
        1,
        OperandDescriptor(TypeData::Data, 1, 0xAA),
        OperandDescriptor(TypeData::Data, 8, 0));

    db.execute(cu);
    EXPECT_EQ(db.getName(), "DB");
}

TEST(Command, DivCommand_execute_divisionByZero_throws)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DivCommand cmd(
        1,
        OperandDescriptor(TypeData::Data, 8, 84),
        OperandDescriptor(TypeData::Data, 8, 0));

    EXPECT_THROW(cmd.execute(cu), std::runtime_error);
}

TEST(Command, DivCommand_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DivCommand cmd(
        1,
        OperandDescriptor(TypeData::Data, 8, 1),
        OperandDescriptor(TypeData::Data, 8, 1));

    EXPECT_EQ(cmd.getName(), "DIV");
}

TEST(Command, CmpCommand_execute_equal_DataData)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    CmpCommand cmd(
        1,
        OperandDescriptor(TypeData::Data, 8, 7),
        OperandDescriptor(TypeData::Data, 8, 7));

    cmd.execute(cu);
    EXPECT_EQ(cmd.operand1.getValue(), 1u);
}

TEST(Command, CmpCommand_execute_notEqual_DataData)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    CmpCommand cmd(
        1,
        OperandDescriptor(TypeData::Data, 8, 7),
        OperandDescriptor(TypeData::Data, 8, 9));

    cmd.execute(cu);
    EXPECT_EQ(cmd.operand1.getValue(), 0u);
}

static void ffff(Bus &bus, uint64_t addr, uint64_t v)
{
    for (int i = 0; i < 8; ++i)
        bus.write(addr + i, (v >> (i * 8)) & 0xFF);
}

TEST(Command, CmpCommand_execute_RegisterRegister)
{
    Bus bus(0x2000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice gpr(256);
    bus.attachDevice("GPR", 0x100, 256, &gpr);

    
    ffff(bus, 0x100 + 1 * 8, 10);
    ffff(bus, 0x100 + 2 * 8, 10);

    CmpCommand cmd(
        1,
        OperandDescriptor(TypeData::Register, 8, 1),
        OperandDescriptor(TypeData::Register, 8, 2));

    cmd.execute(cu);


    EXPECT_EQ(1, 1u);
}

TEST(Command, CmpCommand_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    CmpCommand cmd(
        1,
        OperandDescriptor(TypeData::Data, 8, 1),
        OperandDescriptor(TypeData::Data, 8, 2));

    EXPECT_EQ(cmd.getName(), "CMP");
}


TEST(controleUnit_cpp, ControlUnit_hasActiveThreads)
{
    
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice gpr(16);
    bus.attachDevice("GPR", 0x100, 16, &gpr);

    ProgramMemory pm(64);
    cu.setProgramMemory(&pm);

    EXPECT_FALSE(cu.hasActiveThreads()); 
    SUCCEED();
}