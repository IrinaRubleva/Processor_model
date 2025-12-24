#pragma once
#include "_BinaryCommand.hpp"



/*!
*@brief Команда перемещения. Результат выполнения операции помещается в первый операнд.
*/


struct MovCommand : public BinaryCommand
{
public:
  MovCommand(sizeOfOpcode op, const OperandDescriptor &left, const OperandDescriptor &right)
      : BinaryCommand(op, left, right) {}

  void execute(ControlUnit &cu)
  {
    uint64_t b = operand2.getRawValue(cu.getRefBus());
    std::cout << "[MovCommand] Reading value: " << b << " from operand" << std::endl;
    operand1.setRawValue(cu.getRefBus(), b);
  }

  std::string getName() const override { return "MOV"; }
};
