#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <map>



/*!
*@brief Ассемблер. Превращает текст программы в байт код
*/


class Decoder;
std::string readFileToString(const std::string &filename);

class Assembler
{
public:
    std::vector<uint8_t> programBytes;
    std::map<std::string, uint64_t> label;           // Метки кода → адрес в ProgramMemory
    std::map<std::string, uint64_t> dataVariables;   // Переменные данных → offset в DataMemory
    uint64_t dataSize;                               // Общий размер данных в DataMemory
    uint64_t ThreadCount = 1;
    static Assembler assemble(const std::string &text, Decoder &decoder);
};