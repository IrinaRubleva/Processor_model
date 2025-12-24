#include <gtest/gtest.h>

#include "device/bus.hpp"
#include "device/memoryDevice.hpp"
#include "controleUnit.hpp"
#include "operand.hpp"

#include "commands/JumpCommand/JmpCommand.hpp"
#include "commands/ThreadEndCommand/_ThreadEnd.hpp"
#include "commands/ThreadInitCommand/_ThreadInit.hpp"
#include "commands/UnaryCommand/DecCommand.hpp"
#include <gtest/gtest.h>

#include "device/bus.hpp"
#include "device/memoryDevice.hpp"
#include "controleUnit.hpp"
#include "operand.hpp"

#include "commands/UnaryCommand/IncCommand.hpp"
#include "commands/UnaryCommand/NegCommand.hpp"

#include <gtest/gtest.h>
#include <stdexcept>
#include <thread>
#include <chrono>

#include "device/bus.hpp"
#include "device/memoryDevice.hpp"
#include "controleUnit.hpp"
#include "programMemory.hpp"
#include "operand.hpp"

#include "commands/UnaryCommand/NotCommand.hpp"
#include "commands/registerCommand.hpp"

#include <gtest/gtest.h>

#include "device/bus.hpp"
#include "device/memoryDevice.hpp"
#include "controleUnit.hpp"
#include "programMemory.hpp"
#include "operand.hpp"

#include "commands/DataDeclaration/DdCommand.hpp" // наследник DataDeclarationCommand

// читаем little-endian N байт (1/2/4/8)
static uint64_t read_le(Bus &bus, uint64_t addr, uint8_t n)
{
    uint64_t v = 0;
    for (uint8_t i = 0; i < n; ++i)
        v |= (uint64_t)bus.read(addr + i) << (i * 8);
    return v;
}

TEST(dataDeclaration_cpp, DataDeclarationCommand_compute_noDataMemory_returns)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    // DataMemory НЕ цепляем -> compute должен попасть в catch и просто return
    DdCommand dd(
        1,
        OperandDescriptor(TypeData::Address, 4, (0ull << 8) | 4ull), // не важно, всё равно выйдет раньше
        OperandDescriptor(TypeData::Data, 4, 0x11223344));

    dd.compute(cu);
    SUCCEED();
}

TEST(dataDeclaration_cpp, DataDeclarationCommand_compute_writesBytes)
{
    Bus bus(0x2000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice dataMem(256);
    bus.attachDevice("DataMemory", 0x500, 256, &dataMem);

    // packed address: (offset<<8) | expectedSize
    uint64_t offset = 16;
    uint8_t expectedSize = 4;

    // actualSize берется из valueOperand.getSize()
    // value берется из valueOperand.getValue()
    uint32_t value = 0x11223344;

    DdCommand dd(
        1,
        OperandDescriptor(TypeData::Address, expectedSize, (offset << 8) | expectedSize),
        OperandDescriptor(TypeData::Data, 4, value));

    dd.compute(cu);

    // должно записать 4 байта little-endian в DataMemory base + offset
    EXPECT_EQ(read_le(bus, 0x500 + offset, 4), (uint64_t)value);
}

// чтобы прогнать цикл for ещё раз в другом размере (другая длина цикла)
TEST(dataDeclaration_cpp, DataDeclarationCommand_compute_writesOneByte)
{
    Bus bus(0x2000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice dataMem(256);
    bus.attachDevice("DataMemory", 0x500, 256, &dataMem);

    uint64_t offset = 32;
    uint8_t expectedSize = 1;
    uint8_t value = 0xAB;

    DdCommand dd(
        1,
        OperandDescriptor(TypeData::Address, expectedSize, (offset << 8) | expectedSize),
        OperandDescriptor(TypeData::Data, 1, value));

    dd.compute(cu);

    EXPECT_EQ(bus.read(0x500 + offset), value);
}

// helpers
static void write_u64_le(Bus &bus, uint64_t addr, uint64_t v)
{
    for (int i = 0; i < 8; ++i)
        bus.write(addr + i, (v >> (i * 8)) & 0xFF);
}
static uint64_t read_u64_le(Bus &bus, uint64_t addr)
{
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i)
        v |= (uint64_t)bus.read(addr + i) << (i * 8);
    return v;
}

// -------------------- NotCommand --------------------

TEST(NotCommandCoverage, Not_execute_Data)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    NotCommand cmd(1, OperandDescriptor(TypeData::Data, 8, 0x00FF));
    cmd.execute(cu);
    EXPECT_EQ(cmd.operand.getValue(), ~0x00FFull);
}

TEST(NotCommandCoverage, Not_execute_Register)
{
    Bus bus(0x2000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice gpr(256);
    bus.attachDevice("GPR", 0x100, 256, &gpr);

    write_u64_le(bus, 0x100 + 2 * 8, 0x00FF);
    NotCommand cmd(1, OperandDescriptor(TypeData::Register, 8, 2));
    cmd.execute(cu);

    EXPECT_EQ(read_u64_le(bus, 0x100 + 2 * 8), ~0x00FFull);
}

TEST(NotCommandCoverage, Not_execute_Address)
{
    Bus bus(0x3000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice dataMem(256);
    bus.attachDevice("DataMemory", 0x500, 256, &dataMem);

    uint64_t packedAddr = (16ull << 8) | 8ull; // offset=16, dataSize=8
    write_u64_le(bus, 0x500 + 16, 0x00FF);

    NotCommand cmd(1, OperandDescriptor(TypeData::Address, 8, packedAddr));
    cmd.execute(cu);

    EXPECT_EQ(read_u64_le(bus, 0x500 + 16), ~0x00FFull);
}

TEST(NotCommandCoverage, Not_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    NotCommand cmd(1, OperandDescriptor(TypeData::Data, 8, 1));
    EXPECT_EQ(cmd.getName(), "NOT");
}

// -------------------- registerAllCommands --------------------
// Покрываем: все throw-ветки (ops.size != N) + успешное создание + ThreadEnd dummy.

static CommandFactory buildFactoryWithAllClasses()
{
    CommandFactory f;
    registerAllCommands(f);
    return f;
}

TEST(RegisterAllCommandsCoverage, Add_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("AddCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Add_ok_creates)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("AddCommand");
    std::vector<OperandDescriptor> ops = {
        OperandDescriptor(TypeData::Data, 8, 1),
        OperandDescriptor(TypeData::Data, 8, 2)};
    auto cmd = c(1, ops, 0);
    ASSERT_TRUE(cmd);
}

TEST(RegisterAllCommandsCoverage, Cmp_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("CmpCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Div_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("DivCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Mul_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("MulCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Sub_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("SubCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Mov_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("MovCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Db_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("DbCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 1, 0xAA)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Dd_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("DdCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 4, 0x11223344)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Dq_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("DqCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 0x11)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Dw_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("DwCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 2, 0xBEEF)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Jmp_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("JmpCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1), OperandDescriptor(TypeData::Data, 8, 2)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Inc_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("IncCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1), OperandDescriptor(TypeData::Data, 8, 2)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Neg_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("NegCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1), OperandDescriptor(TypeData::Data, 8, 2)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, Not_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("NotCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1), OperandDescriptor(TypeData::Data, 8, 2)};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, ThreadInit_wrongOps_throws)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("ThreadInitCommand");
    std::vector<OperandDescriptor> ops = {};
    EXPECT_THROW((void)c(1, ops, 0), std::runtime_error);
}

TEST(RegisterAllCommandsCoverage, ThreadEnd_creates_dummy)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("ThreadEndCommand");
    std::vector<OperandDescriptor> ops = {}; // игнорируется
    auto cmd = c(1, ops, 0);
    ASSERT_TRUE(cmd);
}

// ВАЖНО: DecCommand creator НЕБЕЗОПАСЕН для ops.size()!=1 (там закомментирован throw и дальше ops[0]).
// Поэтому для покрытия DecCommand вызываем только с 1 операндом.
TEST(RegisterAllCommandsCoverage, Dec_ok_only)
{
    auto f = buildFactoryWithAllClasses();
    auto c = f.getClass("DecCommand");
    std::vector<OperandDescriptor> ops = {OperandDescriptor(TypeData::Data, 8, 1)};
    auto cmd = c(1, ops, 0);
    ASSERT_TRUE(cmd);
}

// -------------------- ControlUnit.hpp coverage --------------------

TEST(ControlUnitHeaderCoverage, Ctor_withPointers_getPeriod_setPeriod)
{
    Bus bus(0x1000);
    ProgramMemory pm(16);

    ControlUnit cu(&bus, &pm);
    EXPECT_EQ(cu.getPeriod(), 1u);
    cu.setPeriod(7);
    EXPECT_EQ(cu.getPeriod(), 7u);
}

TEST(ControlUnitHeaderCoverage, GetRefBus_throws_whenNull)
{
    ControlUnit cu;
    EXPECT_THROW((void)cu.getRefBus(), std::runtime_error);

    const ControlUnit &ccu = cu;
    EXPECT_THROW((void)ccu.getRefBus(), std::runtime_error);
}

TEST(ControlUnitHeaderCoverage, GetDeviceAddress_ok_and_throw)
{
    Bus bus(0x1000);
    ProgramMemory pm(16);
    ControlUnit cu(&bus, &pm);

    MultiByteMemoryDevice gpr(16);
    bus.attachDevice("GPR", 0x100, 16, &gpr);

    EXPECT_NO_THROW((void)cu.getDeviceAddress("GPR"));
    EXPECT_THROW((void)cu.getDeviceAddress("NO_SUCH_DEVICE"), std::runtime_error);
}

// TEST(ControlUnitHeaderCoverage, Regs_const_and_mut_setReg)
// {
//     try
//     {
//         ControlUnit cu;
//         cu.setReg(0, 123);
//         EXPECT_EQ(cu.getReg(0), 123u);

//         auto &r = cu.regs();
//         (void)r;

//         const ControlUnit &ccu = cu;
//         auto &cr = ccu.regs();
//         (void)cr;
//     }
//     catch (...)
//     {
//     };
// }

TEST(ControlUnitHeaderCoverage, ThreadContext_moveCtor_moveAssign)
{
    ThreadContext a;
    a.pc = 5;
    a.active = false;
    a.should_stop = true;

    ThreadContext b(std::move(a));
    EXPECT_EQ(b.pc, 5u);

    ThreadContext c;
    c = std::move(b);
    EXPECT_EQ(c.pc, 5u);
}

TEST(ControlUnitHeaderCoverage, JoinAllThreads_joinable_and_notJoinable)
{
    ControlUnit cu;

    ThreadContext joinableCtx;
    joinableCtx.thread = std::make_unique<std::thread>([]
                                                       { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });

    ThreadContext notJoinableCtx;
    notJoinableCtx.thread = std::make_unique<std::thread>([] {});
    notJoinableCtx.thread->join(); 

    cu.threads.push_back(std::move(joinableCtx));
    cu.threads.push_back(std::move(notJoinableCtx));

    cu.joinAllThreads(); 
    SUCCEED();
}

TEST(ControlUnitHeaderCoverage, SafeShutdown_runs)
{
    ControlUnit cu;
    cu.safeShutdown();
    EXPECT_TRUE(cu.isFinished());
}


TEST(Command, IncCommand_execute_Data)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    IncCommand inc(1, OperandDescriptor(TypeData::Data, 8, 41));
    inc.execute(cu);

    EXPECT_EQ(inc.operand.getValue(), 42u);
}

TEST(Command, IncCommand_execute_Register)
{
    Bus bus(0x2000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice gpr(256);
    bus.attachDevice("GPR", 0x100, 256, &gpr);

    write_u64_le(bus, 0x100 + 1 * 8, 41);

    IncCommand inc(1, OperandDescriptor(TypeData::Register, 8, 1));
    inc.execute(cu);

    EXPECT_EQ(read_u64_le(bus, 0x100 + 1 * 8), 42u);
}

TEST(Command, IncCommand_execute_Address)
{
    Bus bus(0x3000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice dataMem(256);
    bus.attachDevice("DataMemory", 0x500, 256, &dataMem);

    
    uint64_t packedAddr = (16ull << 8) | 8ull;

    write_u64_le(bus, 0x500 + 16, 41);

    IncCommand inc(1, OperandDescriptor(TypeData::Address, 8, packedAddr));
    inc.execute(cu);

    EXPECT_EQ(read_u64_le(bus, 0x500 + 16), 42u);
}

TEST(Command, IncCommand_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    IncCommand inc(1, OperandDescriptor(TypeData::Data, 8, 1));
    EXPECT_EQ(inc.getName(), "INC");
}

// ---------- NEG ----------
TEST(Command, NegCommand_execute_Data_positive)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    NegCommand neg(1, OperandDescriptor(TypeData::Data, 8, 5));
    neg.execute(cu);

    EXPECT_EQ(neg.operand.getValue(), (uint64_t)(-(int64_t)5));
}

TEST(Command, NegCommand_execute_Data_negative)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    NegCommand neg(1, OperandDescriptor(TypeData::Data, 8, (uint64_t)(-(int64_t)5)));
    neg.execute(cu);

    EXPECT_EQ(neg.operand.getValue(), 5u);
}

TEST(Command, NegCommand_execute_Register)
{
    Bus bus(0x2000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice gpr(256);
    bus.attachDevice("GPR", 0x100, 256, &gpr);

    write_u64_le(bus, 0x100 + 2 * 8, 5);

    NegCommand neg(1, OperandDescriptor(TypeData::Register, 8, 2));
    neg.execute(cu);

    EXPECT_EQ(read_u64_le(bus, 0x100 + 2 * 8), (uint64_t)(-(int64_t)5));
}

TEST(Command, NegCommand_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    NegCommand neg(1, OperandDescriptor(TypeData::Data, 8, 1));
    EXPECT_EQ(neg.getName(), "NEG");
}

// ---------- JMP ----------
TEST(Command, JmpCommand_execute_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    JmpCommand jmp(1, OperandDescriptor(TypeData::Data, 8, 0));
    jmp.execute(cu);                 
    EXPECT_EQ(jmp.getName(), "JMP"); 
}

// ---------- THRE ----------
TEST(Command, ThreadEndCommand_execute_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    ThreadEndCommand cmd(1, OperandDescriptor(TypeData::Data, 0, 0));
    cmd.execute(cu); 
    EXPECT_EQ(cmd.getName(), "THRE");
}

// ---------- THRI ----------
TEST(Command, ThreadInitCommand_execute_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    ThreadInitCommand cmd(1, OperandDescriptor(TypeData::Data, 0, 0));
    cmd.execute(cu); 
    EXPECT_EQ(cmd.getName(), "THRI");
}

// ---------- DEC ----------
TEST(Command, DecCommand_execute_Data)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DecCommand dec(1, OperandDescriptor(TypeData::Data, 8, 10));
    dec.execute(cu);
    EXPECT_EQ(dec.operand.getValue(), 9u);
}

TEST(Command, DecCommand_execute_Register)
{
    Bus bus(0x2000);
    ControlUnit cu;
    cu.setBus(&bus);

    MultiByteMemoryDevice gpr(256);
    bus.attachDevice("GPR", 0x100, 256, &gpr);

    write_u64_le(bus, 0x100 + 1 * 8, 10);

    DecCommand dec(1, OperandDescriptor(TypeData::Register, 8, 1));
    dec.execute(cu);

    EXPECT_EQ(read_u64_le(bus, 0x100 + 1 * 8), 9u);
}

TEST(Command, DecCommand_getName)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    DecCommand dec(1, OperandDescriptor(TypeData::Data, 8, 1));
    EXPECT_EQ(dec.getName(), "DEC");
}
