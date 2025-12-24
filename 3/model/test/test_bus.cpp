#include <gtest/gtest.h>
#include <stdexcept>

#include "test_common.hpp"
#include "device/bus.hpp"
#include "device/memoryDevice.hpp"

TEST(bus_cpp, Bus_read) {
    Bus bus(0x1000);
    MultiByteMemoryDevice mem(16);
    bus.attachDevice("MEM", 0x100, 16, &mem);

    mem.write(0, 0xAA);
    EXPECT_EQ(bus.read(0x100), 0xAA);

    EXPECT_THROW((void)bus.read(0x0), std::runtime_error);       
    EXPECT_THROW((void)bus.read(0x100 + 16), std::runtime_error);
}

TEST(bus_cpp, Bus_write) {
    Bus bus(0x1000);
    MultiByteMemoryDevice mem(4);
    bus.attachDevice("MEM", 0x200, 4, &mem);

    bus.write(0x200, 0x11);
    bus.write(0x203, 0x44);
    EXPECT_EQ(mem.read(0), 0x11);
    EXPECT_EQ(mem.read(3), 0x44);

    EXPECT_THROW(bus.write(0x0, 0xFF), std::runtime_error);
    EXPECT_THROW(bus.write(0x200 + 4, 0xFF), std::runtime_error);
}
