#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <cstdint>
#include <mutex>
#include <condition_variable>


class MockThreadControlUnit
{
private:
    uint32_t threadCount;
    std::mutex mutex;

public:
    explicit MockThreadControlUnit(uint32_t initialCount = 1) : threadCount(initialCount) {}

    uint32_t getThreadCount()
    {
        std::lock_guard<std::mutex> lock(mutex);
        return threadCount;
    }

    void setThreadCount(uint32_t count)
    {
        std::lock_guard<std::mutex> lock(mutex);
        threadCount = count;
    }

    void decrementThreadCount()
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (threadCount > 0)
        {
            threadCount--;
        }
    }
};


class ThreadEndCommand
{
public:
    void compute(MockThreadControlUnit &cu)
    {
        uint32_t c = cu.getThreadCount();
        if (c > 0)
        {
            cu.setThreadCount(c - 1);
        }
        cu.decrementThreadCount(); 
    }
};

TEST(ThreadEndCommandTest, DecrementThreadCount)
{
    MockThreadControlUnit cu(3); 

    ThreadEndCommand cmd;
    cmd.compute(cu);

    EXPECT_EQ(cu.getThreadCount(), 1);
}

TEST(ThreadEndCommandTest, DecrementFromOne)
{
    MockThreadControlUnit cu(1);

    ThreadEndCommand cmd;
    cmd.compute(cu);

    
    EXPECT_EQ(cu.getThreadCount(), 0);
}

TEST(ThreadEndCommandTest, DecrementFromZero)
{
    MockThreadControlUnit cu(0);

    ThreadEndCommand cmd;
    cmd.compute(cu);

    
    EXPECT_EQ(cu.getThreadCount(), 0);
}

TEST(ThreadEndCommandTest, MultipleDecrements)
{
    MockThreadControlUnit cu(5);

    ThreadEndCommand cmd;

    cmd.compute(cu); 
    EXPECT_EQ(cu.getThreadCount(), 3);

    cmd.compute(cu); 
    EXPECT_EQ(cu.getThreadCount(), 1);

    cmd.compute(cu);
    EXPECT_EQ(cu.getThreadCount(), 0);

    cmd.compute(cu); 
    EXPECT_EQ(cu.getThreadCount(), 0);
}