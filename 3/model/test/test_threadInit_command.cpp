// test_thread_init.cpp
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <cstdint>
#include <thread>
#include <mutex>

class MockThreadInitControlUnit {
private:
    std::vector<uint64_t> createdThreads;
    std::mutex mutex;
    uint32_t threadCount;
    
public:
    MockThreadInitControlUnit(uint32_t initialCount = 0) 
        : threadCount(initialCount) {}
    
    void createNewThread(uint64_t addr) {
        std::lock_guard<std::mutex> lock(mutex);
        createdThreads.push_back(addr);
        threadCount++;
    }
    
    std::vector<uint64_t> getCreatedThreads() const {
        return createdThreads;
    }
    
    uint32_t getThreadCount() const {
        return threadCount;
    }
};

class MockThreadInitOperand {
private:
    uint64_t value;
    
public:
    MockThreadInitOperand(uint64_t val = 0) : value(val) {}
    
    uint64_t getValue() const { return value; }
};

class ThreadInitCommand {
private:
    MockThreadInitOperand operand;
    
public:
    ThreadInitCommand(MockThreadInitOperand op) : operand(op) {}
    
    void compute(MockThreadInitControlUnit& cu) {
        uint64_t addr = operand.getValue();
        cu.createNewThread(addr);
    }
};

TEST(ThreadInitCommandTest, CreateThreadAtAddress) {
    MockThreadInitControlUnit cu;
    MockThreadInitOperand op(0x100);
    
    ThreadInitCommand thriCmd(op);
    thriCmd.compute(cu);
    
    auto threads = cu.getCreatedThreads();
    ASSERT_EQ(threads.size(), 1);
    EXPECT_EQ(threads[0], 0x100);
    EXPECT_EQ(cu.getThreadCount(), 1);
}

TEST(ThreadInitCommandTest, CreateMultipleThreads) {
    MockThreadInitControlUnit cu;
    
    MockThreadInitOperand op1(0x100);
    MockThreadInitOperand op2(0x200);
    MockThreadInitOperand op3(0x300);
    
    ThreadInitCommand cmd1(op1);
    ThreadInitCommand cmd2(op2);
    ThreadInitCommand cmd3(op3);
    
    cmd1.compute(cu);
    cmd2.compute(cu);
    cmd3.compute(cu);
    
    auto threads = cu.getCreatedThreads();
    ASSERT_EQ(threads.size(), 3);
    EXPECT_EQ(threads[0], 0x100);
    EXPECT_EQ(threads[1], 0x200);
    EXPECT_EQ(threads[2], 0x300);
    EXPECT_EQ(cu.getThreadCount(), 3);
}

TEST(ThreadInitCommandTest, CreateThreadAtZero) {
    MockThreadInitControlUnit cu;
    MockThreadInitOperand op(0);
    
    ThreadInitCommand thriCmd(op);
    thriCmd.compute(cu);
    
    auto threads = cu.getCreatedThreads();
    ASSERT_EQ(threads.size(), 1);
    EXPECT_EQ(threads[0], 0);
    EXPECT_EQ(cu.getThreadCount(), 1);
}