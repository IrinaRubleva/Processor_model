#include <gtest/gtest.h>
#include <memory>
#include "decoder.hpp"
#include "programMemory.hpp"
#include "commands/command.hpp"
#include "operand.hpp"

namespace {
struct FakeCmd : public Command {
    void execute(ControlUnit&) override {}
    void compute(ControlUnit&) override {}
    std::string getName() const override { return "FAKE"; }
};
}

TEST(decoder_cpp, Decoder_decode) {
    ProgramMemory pm(32);
    Decoder dec;

    
    dec.getFactory().registerOpcode(
        1, "FAKE",
        [](sizeOfOpcode, const std::vector<OperandDescriptor>&, uint64_t) {
            return std::make_unique<FakeCmd>();
        }
    );


    pm.write(0, 0xFF);
    pm.write(1, 0);
    uint64_t pc0 = 0;
    EXPECT_EQ(dec.decode(pc0, pm), nullptr);

    uint64_t pc = 0;
    pm.write(pc++, 1); 
    pm.write(pc++, 1); 
    pm.write(pc++, static_cast<uint8_t>(TypeData::Data));
    pm.write(pc++, 4);
    pm.write(pc++, 0x44);
    pm.write(pc++, 0x33);
    pm.write(pc++, 0x22);
    pm.write(pc++, 0x11);

    uint64_t pc1 = 0;
    auto cmd = dec.decode(pc1, pm);
    ASSERT_NE(cmd, nullptr);
    EXPECT_EQ(cmd->getName(), "FAKE");
    EXPECT_EQ(pc1, 8u);
}
