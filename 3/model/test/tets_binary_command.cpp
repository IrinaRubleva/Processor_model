#include <gtest/gtest.h>
#include "test_common.hpp"



TEST(binaryCommand_cpp, BinaryCommand_compute) {
    CUEnv env;

    auto& factory = env.cu.getRefDecoder().getFactory();

    factory.registerOpcode(1, "MOV", factory.getClass("MovCommand"));
    factory.registerOpcode(2, "CMP", factory.getClass("CmpCommand"));
    factory.registerOpcode(3, "ADD", factory.getClass("AddCommand"));

    {
        MovCommand mov(1, OperandDescriptor(TypeData::Data, 8, 0), OperandDescriptor(TypeData::Data, 8, 123));
        mov.compute(env.cu);
        EXPECT_EQ(mov.operand1.getValue(), 123u);
    }

 
    {
        CmpCommand cmp(2, OperandDescriptor(TypeData::Data, 8, 7), OperandDescriptor(TypeData::Data, 8, 7));
        cmp.compute(env.cu);
        EXPECT_EQ(cmp.operand1.getValue(), 1u);
    }

  
    {
        AddCommand add(3, OperandDescriptor(TypeData::Data, 8, 10), OperandDescriptor(TypeData::Data, 8, 32));
        add.compute(env.cu);
        EXPECT_EQ(add.operand1.getValue(), 42u);
    }

    {
        AddCommand bad(99, OperandDescriptor(TypeData::Data, 8, 1), OperandDescriptor(TypeData::Data, 8, 2));
        EXPECT_THROW(bad.compute(env.cu), std::exception);
    }
}
