#include <gtest/gtest.h>
#include "test_common.hpp"


TEST(unaryCommand_cpp, UnaryCommand_compute) {
    CUEnv env;
    auto& factory = env.cu.getRefDecoder().getFactory();
    factory.registerOpcode(10, "INC", factory.getClass("IncCommand"));

    IncCommand inc(10, OperandDescriptor(TypeData::Data, 8, 41));
    inc.compute(env.cu);
    EXPECT_EQ(inc.operand.getValue(), 42u);

    IncCommand bad(77, OperandDescriptor(TypeData::Data, 8, 1));
    EXPECT_THROW(bad.compute(env.cu), std::exception);
}
