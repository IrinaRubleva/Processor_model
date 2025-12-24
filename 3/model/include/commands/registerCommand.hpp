#pragma once

#include "command.hpp"

#include "BinaryCommand/AddCommand.hpp"
#include "BinaryCommand/CmpCommand.hpp"
#include "BinaryCommand/DivCommand.hpp"
#include "BinaryCommand/MovCommand.hpp"
#include "BinaryCommand/MulCommand.hpp"
#include "BinaryCommand/SubCommand.hpp"

#include "DataDeclaration/DbCommand.hpp"
#include "DataDeclaration/DdCommand.hpp"
#include "DataDeclaration/DqCommand.hpp"
#include "DataDeclaration/DwCommand.hpp"

#include "JumpCommand/JmpCommand.hpp"

#include "UnaryCommand/DecCommand.hpp"
#include "UnaryCommand/IncCommand.hpp"
#include "UnaryCommand/NegCommand.hpp"
#include "UnaryCommand/NotCommand.hpp"

#include "ThreadEndCommand/_ThreadEnd.hpp"
#include "ThreadInitCommand/_ThreadInit.hpp"







/*!
*@brief Реестр команд. Для добавления новой команды пользователю надо прописать  registry.registerClass(...), а также добавить в 
*   конфигурационный файл класс новой команды и её опкод
*/ 



inline void registerAllCommands(CommandFactory &registry)
{
    registry.registerClass("AddCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("AddCommand expects 2 operands");

            return std::make_unique<AddCommand>(op, ops[0], ops[1]); });

    registry.registerClass("CmpCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("CmpCommand expects 2 operands");

            return std::make_unique<CmpCommand>(op, ops[0], ops[1]); });

    registry.registerClass("DivCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("DivCommand expects 2 operands");

            return std::make_unique<DivCommand>(op, ops[0], ops[1]); });

    registry.registerClass("MulCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("MulCommand expects 2 operands");

            return std::make_unique<MulCommand>(op, ops[0], ops[1]); });

    registry.registerClass("SubCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("SubCommand expects 2 operands");

            return std::make_unique<SubCommand>(op, ops[0], ops[1]); });

    registry.registerClass("MovCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("MovCommand expects 2 operands");

            return std::make_unique<MovCommand>(op, ops[0], ops[1]); });

    registry.registerClass("DbCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("DbCommand expects 2 operands");

            return std::make_unique<DbCommand>(op, ops[0], ops[1]); });

    registry.registerClass("DdCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t pc)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("DdCommand expects 2 operands");

            return std::make_unique<DdCommand>(op, ops[0], ops[1]); });

    registry.registerClass("DqCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("DqCommand expects 2 operands");

            return std::make_unique<DqCommand>(op, ops[0], ops[1]); });

    registry.registerClass("DwCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 2)
                throw std::runtime_error("DwCommand expects 2 operands");

            return std::make_unique<DwCommand>(op, ops[0], ops[1]); });

    registry.registerClass("JmpCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 1)
                throw std::runtime_error("JmpCommand expects 2 operands");

            return std::make_unique<JmpCommand>(op, ops[0]); });

    registry.registerClass("DecCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 1)
             //   throw std::runtime_error("DecCommand expects 2 operands");

            return std::make_unique<DecCommand>(op, ops[0]); });

    registry.registerClass("IncCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 1)
                throw std::runtime_error("IncCommand expects 2 operands");

            return std::make_unique<IncCommand>(op, ops[0]); });

    registry.registerClass("NegCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 1)
                throw std::runtime_error("NegCommand expects 2 operands");

            return std::make_unique<NegCommand>(op, ops[0]); });

    registry.registerClass("NotCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 1)
                throw std::runtime_error("NotCommand expects 2 operands");

            return std::make_unique<NotCommand>(op, ops[0]); });

    registry.registerClass("ThreadInitCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            if (ops.size() != 1)
                throw std::runtime_error("ThreadInitCommand expects 2 operands");

            return std::make_unique<ThreadInitCommand>(op, ops[0]); });

    registry.registerClass("ThreadEndCommand", [](sizeOfOpcode op, const std::vector<OperandDescriptor> &ops, uint64_t)
                           {
            OperandDescriptor dummy(TypeData::Data, 0, 0);
            return std::make_unique<ThreadEndCommand>(op, dummy); });
}
