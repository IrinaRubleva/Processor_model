// test_jump_command.cpp
#include <gtest/gtest.h>
#include <memory>
#include <cstdint>

class MockJumpControlUnit {
private:
    uint64_t pc;
    
public:
    MockJumpControlUnit(uint64_t initialPC = 0) : pc(initialPC) {}
    
    uint64_t getPC() const { return pc; }
    void setPC(uint64_t newPC) { pc = newPC; }
};

class MockJumpOperand {
private:
    uint64_t value;
    
public:
    MockJumpOperand(uint64_t val = 0) : value(val) {}
    
    uint64_t getValue() const { return value; }
};

class JumpCommand {
private:
    MockJumpOperand operand;
    
public:
    JumpCommand(MockJumpOperand op) : operand(op) {}
    
    void compute(MockJumpControlUnit& cu) {
        uint64_t addr = operand.getValue();
        cu.setPC(addr);
    }
};

TEST(JumpCommandTest, JumpToAddress) {
    MockJumpControlUnit cu(100);  
    MockJumpOperand op(500);     
    
    JumpCommand jumpCmd(op);
    jumpCmd.compute(cu);
    
    EXPECT_EQ(cu.getPC(), 500);
}

TEST(JumpCommandTest, JumpToZero) {
    MockJumpControlUnit cu(300);
    MockJumpOperand op(0);
    
    JumpCommand jumpCmd(op);
    jumpCmd.compute(cu);
    
    EXPECT_EQ(cu.getPC(), 0);
}

TEST(JumpCommandTest, MultipleJumps) {
    MockJumpControlUnit cu(0);
    MockJumpOperand op1(100);
    MockJumpOperand op2(200);
    MockJumpOperand op3(300);
    
    JumpCommand jump1(op1);
    jump1.compute(cu);
    EXPECT_EQ(cu.getPC(), 100);
    
    JumpCommand jump2(op2);
    jump2.compute(cu);
    EXPECT_EQ(cu.getPC(), 200);
    
    JumpCommand jump3(op3);
    jump3.compute(cu);
    EXPECT_EQ(cu.getPC(), 300);
}

TEST(JumpCommandTest, JumpToSameAddress) {
    MockJumpControlUnit cu(150);
    MockJumpOperand op(150);  
    
    JumpCommand jumpCmd(op);
    jumpCmd.compute(cu);
    
    EXPECT_EQ(cu.getPC(), 150);
}