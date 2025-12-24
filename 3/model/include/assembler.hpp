#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

class Decoder;
std::string readFileToString(const std::string &filename);

class Assembler
{
public:
    std::vector<uint8_t> programBytes;
    std::map<std::string, uint64_t> label;           // Метки кода → адрес в ProgramMemory
    std::map<std::string, uint64_t> dataVariables;   // Переменные данных → offset в DataMemory
    uint64_t dataSize;                               // Общий размер данных в DataMemory

    static Assembler assemble(const std::string &text, Decoder &decoder);
};