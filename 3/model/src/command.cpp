#include "../include/command.hpp"


//отправляем данные в ALU через УУ и Шину

void UnaryCommand::execute(Bus& bus, ControlUnit& cu) {
 //   uint64_t aluBase = cu.getDeviceAddress("ALU");
 //   bus.write(aluBase + 0, static_cast<uint8_t>(type));


}


void BinaryCommand::execute(Bus& bus, ControlUnit& cu) {
   uint64_t aluBase = cu.getDeviceAddress("ALU");
}


void JumpCommand::execute(Bus& bus, ControlUnit& cu) {
   // cu.setPC(operand.getRawValue(bus));
}


void DataDeclarationCommand::execute(Bus& bus, ControlUnit& cu) {}


void ThreadInitCommand::execute(Bus& bus, ControlUnit& cu) {}


void ThreadEndCommand::execute(Bus& bus, ControlUnit& cu) {}