#include <gtest/gtest.h>
#include "test_common.hpp"

#include "commands/JumpCommand/JmpCommand.hpp"
#include "commands/ThreadInitCommand/_ThreadInit.hpp"
#include "commands/ThreadEndCommand/_ThreadEnd.hpp"

TEST(jumpCommand_cpp, JumpCommand_compute) {
    CUEnv env;
    JmpCommand jmp(1, OperandDescriptor(TypeData::Address, 8, 123));
    jmp.compute(env.cu);
    EXPECT_EQ(env.cu.getPC(), 123u);
}

TEST(ThreadInit_cpp, ThreadInitCommand_compute) {
    CUEnv env(0); 
    ThreadInitCommand thri(1, OperandDescriptor(TypeData::Address, 8, 999));
    thri.compute(env.cu);
    SUCCEED();
}

TEST(ThreadEnd_cpp, ThreadEndCommand_compute) {
    CUEnv env;

    env.gpr.write(0, 2); env.gpr.write(1, 0); env.gpr.write(2, 0); env.gpr.write(3, 0);

    ThreadEndCommand thre(1, OperandDescriptor(TypeData::Data, 0, 0));
    thre.compute(env.cu);

    EXPECT_EQ(env.gpr.read(0), 1);
}
