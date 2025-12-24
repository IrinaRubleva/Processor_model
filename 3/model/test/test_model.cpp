#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <cstdint>

class Bus;
class ControlUnit;
class Processor;
class Decoder;
class Command;
class Operand;
class ProgramMemory;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}