#include "assembler.hpp"
#include "../include/operand.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "../include/decoder.hpp"
#include <unordered_map>

std::string readFileToString(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static bool isRegister(const std::string &s)
{
    return s.size() >= 2 && s[0] == 'R' && std::isdigit(s[1]);
}

static uint64_t parseRegister(const std::string &s)
{
    return std::stoull(s.substr(1));
}

static bool isNumber(const std::string &s)
{
    if (s.empty())
        return false;
    if (s[0] == '-')
        return s.size() > 1 && std::isdigit(s[1]);
    return std::isdigit(s[0]);
}

static std::string trim(const std::string &s)
{
    size_t b = s.find_first_not_of(" \t");
    size_t e = s.find_last_not_of(" \t");
    if (b == std::string::npos)
        return "";
    return s.substr(b, e - b + 1);
}

Assembler Assembler::assemble(const std::string &text, Decoder &decoder)
{
    Assembler out;

    std::stringstream ss(text);
    std::string line;
    uint64_t pc = 0;         
    uint64_t dataOffset = 0; 

    std::map<std::string, uint64_t> codeLabels;    
    std::map<std::string, uint64_t> dataVariables; 
    std::map<std::string, uint8_t> dataSizes;   
    
    uint64_t ThrCount = 1;

    
    while (std::getline(ss, line))
    {
        line = trim(line);
        if (line.empty())
            continue;

        // Обработка меток (label:)
        if (line.back() == ':')
        {
            std::string label = trim(line.substr(0, line.size() - 1));
            codeLabels[label] = pc;
            continue;
        }

        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ls(line);

        std::string cmd;
        ls >> cmd;

        if (!cmd.empty())
        {
            // -------- DATA DECLARATION --------
            if (cmd == "DB" || cmd == "DW" || cmd == "DD" || cmd == "DQ")
            {
                std::string varName;
                uint64_t value;
                if (ls >> varName >> value)
                {
                    dataVariables[varName] = dataOffset;

                    uint8_t size = (cmd == "DB") ? 1 : (cmd == "DW") ? 2
                                                   : (cmd == "DD")   ? 4
                                                                     : 8;
                    dataSizes[varName] = size;

                    dataOffset += size;
                    pc += 1 + 1 + 10 + (2 + size); // opcode + count + address operand + data operand
                }
                continue;
            }

            // Подсчитываем размер команды
            pc += 2; // opcode + operand_count
            
            // Читаем оставшиеся токены для подсчета операндов
            std::vector<std::string> tokens;
            std::string token;
            while (ls >> token) {
                tokens.push_back(token);
            }
            
            // Подсчет размера операндов
            for (const auto& token : tokens) {
                if (isRegister(token)) {
                    pc += 1 + 1 + 4; 
                } else if (isNumber(token)) {
                    pc += 1 + 1 + 4; 
                } else if (codeLabels.count(token) || dataVariables.count(token)) {
                    pc += 1 + 1 + 8; 
                } else if (!token.empty()) {
                    // Предполагаем, что это метка или переменная, которые будут определены позже
                    pc += 1 + 1 + 8;
                }
            }
        }
    }

    // ВТОРОЙ ПРОХОД: генерация байткода
    ss.clear();
    ss.seekg(0);
    pc = 0;

    auto &opcodeToNameMap = decoder.getFactory().getOpcodeMap();
    
    //std::cout << "[Assembler] Assembling with " << opcodeToNameMap.size() << " opcodes" << std::endl;
    //std::cout << "[Assembler] Data variables found: " << dataVariables.size() << std::endl;
    for (const auto &[name, addr] : dataVariables)
    {
        //std::cout << "  " << name << " -> " << addr << " (size: " << (int)dataSizes[name] << ")" << std::endl;
    }
    //std::cout << "[Assembler] Code labels found: " << codeLabels.size() << std::endl;

    auto emitOperand = [&](TypeData type, uint8_t size, uint64_t value)
    {
        //std::cout << "[Assembler] emitOperand: type=" << static_cast<int>(type) << ", size=" << (int)size << ", value=" << value << std::endl;

        out.programBytes.push_back(static_cast<uint8_t>(type));
        out.programBytes.push_back(size);
        for (int i = 0; i < size; ++i)
            out.programBytes.push_back((value >> (i * 8)) & 0xFF);
    };

    while (std::getline(ss, line))
    {
        line = trim(line);
        if (line.empty())
            continue;

        // Пропускаем метки во втором проходе
        if (line.back() == ':')
            continue;

        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ls(line);

        std::string cmd;
        ls >> cmd;

        // Находим opcode по имени команды
        auto it = std::find_if(opcodeToNameMap.begin(), opcodeToNameMap.end(),
            [&cmd](const auto& pair) { 
                return pair.second == cmd; 
            });

        if (it == opcodeToNameMap.end())
        {
            //std::cout << "[Assembler] ERROR: Unknown opcode '" << cmd << "'" << std::endl;
            throw std::runtime_error("Unknown opcode: " + cmd);
        }

        uint8_t opcode = it->first;
        
        //std::cout << "\n[Assembler] Command: " << cmd << " (opcode=" << (int)opcode << ")" << std::endl;

        // -------- DATA DECLARATION --------
        if (cmd == "DB" || cmd == "DW" || cmd == "DD" || cmd == "DQ")
        {
            out.programBytes.push_back(opcode);
            
            std::string name;
            uint64_t value;
            ls >> name >> value;

            uint8_t size = (cmd == "DB") ? 1 : (cmd == "DW") ? 2
                                           : (cmd == "DD")   ? 4
                                                             : 8;

            out.programBytes.push_back(2);
            
            // Первый операнд: адрес переменной
            uint64_t packed = (dataVariables[name] << 8) | size;
            emitOperand(TypeData::Address, 8, packed);
            
            // Второй операнд: значение
            emitOperand(TypeData::Data, size, value);

            continue;
        }

        // Читаем оставшиеся токены
        std::vector<std::string> tokens;
        std::string token;
        while (ls >> token) {
            tokens.push_back(token);
        }

        //std::cout << "[Assembler] Tokens found: ";
        for (const auto& t : tokens) //std::cout << "'" << t << "' ";
        //std::cout << std::endl;

        // -------- КОМАНДЫ БЕЗ ОПЕРАНДОВ (THRE) --------
        if (tokens.empty() && cmd == "THRE") {
            //std::cout << "[Assembler] No-operand command: " << cmd << std::endl;
            out.programBytes.push_back(opcode);
            out.programBytes.push_back(0); // 0 операндов
            continue;
        }

        // -------- JUMP / THREAD INIT --------
        if (cmd == "JMP" || cmd == "THRI") {
            if (tokens.size() != 1) {
                throw std::runtime_error(cmd + " requires exactly 1 operand (label)");
            }
            
            std::string label = tokens[0];
            if (!codeLabels.count(label)) {
                throw std::runtime_error("Unknown label: " + label);
            }

            out.programBytes.push_back(opcode);
            out.programBytes.push_back(1); // 1 операнд
            emitOperand(TypeData::Address, 8, codeLabels[label]);
            continue;
            if(cmd == "THRI") ++ThrCount;

        }

        // -------- UNARY COMMANDS (INC, DEC, NEG, NOT) --------
        if (tokens.size() == 1) {
            out.programBytes.push_back(opcode);
            out.programBytes.push_back(1);

            std::string operand = tokens[0];
            
            if (isRegister(operand)) {
                emitOperand(TypeData::Register, 4, parseRegister(operand));
            }
            else if (isNumber(operand)) {
                emitOperand(TypeData::Data, 4, std::stoull(operand));
            }
            else if (codeLabels.count(operand)) {
                emitOperand(TypeData::Address, 8, codeLabels[operand]);
            }
            else if (dataVariables.count(operand)) {
                uint64_t packed = (dataVariables[operand] << 8) | dataSizes[operand];
                emitOperand(TypeData::Address, 8, packed);
            }
            else {
                throw std::runtime_error("Invalid operand: " + operand);
            }
            continue;
        }

        // -------- BINARY COMMANDS (ADD, SUB, MUL, DIV, MOV, CMP) --------
        if (tokens.size() == 2) {
            out.programBytes.push_back(opcode);
            out.programBytes.push_back(2); // 2 операнда

            auto emitAuto = [&](const std::string &s)
            {
                //std::cout << "[Assembler] Processing operand: '" << s << "'" << std::endl;

                if (isRegister(s))
                {
                    //std::cout << "  → Type: Register" << std::endl;
                    emitOperand(TypeData::Register, 4, parseRegister(s));
                }
                else if (isNumber(s))
                {
                    //std::cout << "  → Type: Data (immediate)" << std::endl;
                    emitOperand(TypeData::Data, 4, std::stoull(s));
                }
                else if (codeLabels.count(s))
                {
                    //std::cout << "  → Type: Address (code label at " << codeLabels[s] << ")" << std::endl;
                    emitOperand(TypeData::Address, 8, codeLabels[s]);
                }
                else if (dataVariables.count(s))
                {
                    //std::cout << "  → Type: Address (data variable at offset " << dataVariables[s] << ", size " << (int)dataSizes[s] << ")" << std::endl;
                    
                    uint64_t packed = (dataVariables[s] << 8) | dataSizes[s];
                    emitOperand(TypeData::Address, 8, packed);
                }
                else
                {
                    throw std::runtime_error("Unknown operand: " + s);
                }
            };

            emitAuto(tokens[0]);
            emitAuto(tokens[1]);
            continue;
        }

        // Если дошли сюда - ошибка формата
        //throw std::runtime_error("Invalid command format: " + cmd);
    }

    out.dataVariables = dataVariables;
    out.label = codeLabels;
    out.dataSize = dataOffset;
    out.ThreadCount = ThrCount;

    //std::cout << "[Assembler] Assembly complete. Program size: " << out.programBytes.size() << " bytes, Data size: " << dataOffset << " bytes" << std::endl;
    return out;
}