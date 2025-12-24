#include <gtest/gtest.h>
#include "test_common.hpp"

#include "commands/BinaryCommand/AddCommand.hpp"
#include "commands/UnaryCommand/IncCommand.hpp"

static uint64_t readU64FromSync(ALUDevice_Synchron& alu) {
    uint64_t x = 0;
    for (int i = 0; i < 8; ++i) x |= (uint64_t)alu.read(17 + i) << (i * 8);
    return x;
}

TEST(alu_cpp, ALUDevice_Synchron_read) {
    ALUDevice_Synchron alu(1);
    alu.write(0, 0x7F);
    EXPECT_EQ(alu.read(0), 0x7F);
    EXPECT_EQ(alu.read(999), 0xFF);
}

TEST(alu_cpp, ALUDevice_Synchron_write) {
    ALUDevice_Synchron alu(1);
    alu.write(0, 1);
    alu.write(1, 0xAA);
    alu.write(9, 0xBB);
    EXPECT_EQ(alu.read(1), 0xAA);
    EXPECT_EQ(alu.read(9), 0xBB);
}

TEST(alu_cpp, ALUDevice_Synchron_tick) {
    CUEnv env;

    // early return when no cmd
    env.alu.setCurrentCommand(nullptr);
    env.alu.tick(env.cu);

    // binary
    AddCommand add(1, OperandDescriptor(TypeData::Data, 8, 0), OperandDescriptor(TypeData::Data, 8, 0));
    for (int i = 0; i < 8; ++i) {
        env.alu.write(1 + i, (10ull >> (i * 8)) & 0xFF);
        env.alu.write(9 + i, (32ull >> (i * 8)) & 0xFF);
    }
    env.alu.setCurrentCommand(&add);
    env.alu.tick(env.cu);
    EXPECT_EQ(readU64FromSync(env.alu), 42u);

    // unary
    IncCommand inc(2, OperandDescriptor(TypeData::Data, 8, 0));
    for (int i = 0; i < 8; ++i) env.alu.write(1 + i, (41ull >> (i * 8)) & 0xFF);
    env.alu.setCurrentCommand(&inc);
    env.alu.tick(env.cu);
    EXPECT_EQ(readU64FromSync(env.alu), 42u);
}

// TEST(alu_cpp, ALUDevice_Synchron_unpack) {
//     ALUDevice_Synchron alu(1);
//     std::array<uint8_t,8> a{};
//     a[0]=0x78; a[1]=0x56; a[2]=0x34; a[3]=0x12;
//     EXPECT_EQ(alu.unpack(a), 0x12345678ull);
// }

// TEST(alu_cpp, ALUDevice_Synchron_pack) {
//     ALUDevice_Synchron alu(1);
//     std::array<uint8_t,8> dst{};
//     alu.pack(dst, 0x1122334455667788ull);
//     EXPECT_EQ(dst[0], 0x88);
//     EXPECT_EQ(dst[7], 0x11);
// }

// ---- Async ----

TEST(alu_cpp, ALUDevice_Asynchron_read) {
    ALUDevice_Asynchron alu;
    alu.write(0, 0xAA);
    EXPECT_EQ(alu.read(0), 0xAA);
    EXPECT_EQ(alu.read(999), 0);
}

TEST(alu_cpp, ALUDevice_Asynchron_write) {
    ALUDevice_Asynchron alu;
    alu.write(0, 1);
    alu.write(1, 0x11);
    alu.write(16, 0x22); // set is_Ready=true
    SUCCEED();
}

TEST(alu_cpp, ALUDevice_Asynchron_tick) {
    CUEnv env;
    ALUDevice_Asynchron alu;

    // not ready -> return
    alu.setCurrentCommand(nullptr);
    alu.tick(env.cu);

    // ready + binary
    AddCommand add(1, OperandDescriptor(TypeData::Data, 8, 0), OperandDescriptor(TypeData::Data, 8, 0));
    alu.setCurrentCommand(&add);

    for (int i = 0; i < 8; ++i) {
        alu.write(1 + i, (10ull >> (i * 8)) & 0xFF);
        alu.write(9 + i, (32ull >> (i * 8)) & 0xFF);
    }
    alu.write(16, 0x00); // последняя запись -> ready

    alu.tick(env.cu);
    uint64_t res = 0;
    for (int i = 0; i < 8; ++i) res |= (uint64_t)alu.read(17 + i) << (i * 8);
    EXPECT_EQ(res, 42u);

    // ready + unary
    IncCommand inc(2, OperandDescriptor(TypeData::Data, 8, 0));
    alu.setCurrentCommand(&inc);
    for (int i = 0; i < 8; ++i) alu.write(1 + i, (41ull >> (i * 8)) & 0xFF);
    for (int i = 0; i < 8; ++i) alu.write(9 + i, 0);
    alu.write(16, 0x00);

    alu.tick(env.cu);
    res = 0;
    for (int i = 0; i < 8; ++i) res |= (uint64_t)alu.read(17 + i) << (i * 8);
    EXPECT_EQ(res, 42u);
}

// TEST(alu_cpp, ALUDevice_Asynchron_unpack) {
//     ALUDevice_Asynchron alu;
//     std::array<uint8_t,8> a{};
//     a[0]=0xEF; a[1]=0xCD; a[2]=0xAB; a[3]=0x89;
//     EXPECT_EQ(alu.unpack(a), 0x89ABCDEFull);
// }

// TEST(alu_cpp, ALUDevice_Asynchron_pack) {
//     ALUDevice_Asynchron alu;
//     std::array<uint8_t,8> dst{};
//     alu.pack(dst, 0x0102030405060708ull);
//     EXPECT_EQ(dst[0], 0x08);
//     EXPECT_EQ(dst[7], 0x01);
// }
