#include <gtest/gtest.h>
#include "../include/processor.hpp"
#include "../include/loader.hpp"

#include "../include/commands/BinaryCommand/AddCommand.hpp"
#include "../include/commands/BinaryCommand/CmpCommand.hpp"
#include "../include/commands/BinaryCommand/DivCommand.hpp"
#include "../include/commands/BinaryCommand/MovCommand.hpp"
#include "../include/commands/BinaryCommand/MulCommand.hpp"
#include "../include/commands/BinaryCommand/SubCommand.hpp"

#include "../include/commands/DataDeclaration/DbCommand.hpp"
#include "../include/commands/DataDeclaration/DdCommand.hpp"
#include "../include/commands/DataDeclaration/DqCommand.hpp"
#include "../include/commands/DataDeclaration/DwCommand.hpp"

#include "../include/commands/JumpCommand/JmpCommand.hpp"

#include "../include/commands/UnaryCommand/DecCommand.hpp"
#include "../include/commands/UnaryCommand/IncCommand.hpp"
#include "../include/commands/UnaryCommand/NegCommand.hpp"
#include "../include/commands/UnaryCommand/NotCommand.hpp"

#include "../include/commands/ThreadEndCommand/_ThreadEnd.hpp"
#include "../include/commands/ThreadInitCommand/_ThreadInit.hpp"

TEST(Command, AddCommand)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    AddCommand cmd(1,
                   OperandDescriptor(TypeData::Data, 8, 10),
                   OperandDescriptor(TypeData::Data, 8, 32));

    cmd.execute(cu);
    EXPECT_EQ(cmd.operand1.getValue(), 42u);
}

TEST(Command, SubCommand)
{
    Bus bus(0x1000);
    ControlUnit cu;
    cu.setBus(&bus);

    SubCommand cmd(1,
                   OperandDescriptor(TypeData::Data, 8, 10),
                   OperandDescriptor(TypeData::Data, 8, 3));

    cmd.execute(cu);
    EXPECT_EQ(cmd.operand1.getValue(), 7u);
}
