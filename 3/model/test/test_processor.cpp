#include <gtest/gtest.h>
#include "../include/processor.hpp"
#include "../include/device/bus.hpp"
#include "../include/device/memoryDevice.hpp"
#include <memory>
#include <thread>

class ProcessorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        processor = std::make_unique<Processor>(1024, 16);

        
        auto &bus = *processor->getBus();

        
        auto gpr = std::make_unique<SingleByteMemoryDevice>(16);
        bus.attachDevice("GPR", 0x100, 16, gpr.get());
        gprDevice = std::move(gpr);

       
        auto alu = std::make_unique<ALUDevice_Synchron>(10);
        bus.attachDevice("ALU", 0x200, 32, alu.get());
        aluDevice = std::move(alu);

        
        auto dataMem = std::make_unique<MultiByteMemoryDevice>(256);
        bus.attachDevice("DataMemory", 0x300, 256, dataMem.get());
        dataMemory = std::move(dataMem);
    }

    std::unique_ptr<Processor> processor;
    std::unique_ptr<SingleByteMemoryDevice> gprDevice;
    std::unique_ptr<ALUDevice_Synchron> aluDevice;
    std::unique_ptr<MultiByteMemoryDevice> dataMemory;
};

TEST_F(ProcessorTest, ConstructorInitializesComponents)
{
    EXPECT_NE(processor->getBus(), nullptr);
    EXPECT_NE(processor->getControlUnit(), nullptr);
    EXPECT_EQ(processor->getCurrentTime(), 0);
}

TEST_F(ProcessorTest, AttachDevice)
{
    auto device = std::make_unique<SingleByteMemoryDevice>(256);
    auto *devicePtr = device.get();

    EXPECT_NO_THROW({
        processor->attachDevice("TestDevice", 0x400, 256, std::move(device));
    });

    
    auto retrieved = processor->getBus()->getDevice<ReadableDevice>("TestDevice");
    EXPECT_TRUE(retrieved.has_value());
    EXPECT_EQ(*retrieved, devicePtr);
}

TEST_F(ProcessorTest, CurrentTimeIncrements)
{
    uint64_t initialTime = processor->getCurrentTime();
    EXPECT_NO_THROW({
        uint64_t time = processor->getCurrentTime();
        EXPECT_EQ(time, initialTime);
    });
}

TEST_F(ProcessorTest, BusAddressSpace)
{
    EXPECT_EQ(processor->getBus()->getAddressSpaceSize(), 1024);
}

TEST_F(ProcessorTest, AttachDeviceToProcessor)
{
    auto device = std::make_unique<SingleByteMemoryDevice>(128);

    EXPECT_NO_THROW({
        processor->attachDevice("NewDevice", 0x500, 128, std::move(device));
    });

    EXPECT_TRUE(processor->getBus()->getDevice<ReadableDevice>("NewDevice").has_value());
}
