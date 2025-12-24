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

 
    auto getOperandSize = [&](const std::string &s) -> uint8_t {
        if (isRegister(s)) return 4;          
        if (isNumber(s)) return 4;            
        if (dataVariables.count(s)) return 8; 
        if (codeLabels.count(s)) return 8;    
        return 8;                             
    };

    while (std::getline(ss, line))
    {
        line = trim(line);
        if (line.empty())
            continue;

        // label:
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
                    pc += 1 + 1 + 10 + (2 + size);
                }
                continue;
            }

            pc += 2;
            std::string a1, a2;
            ls >> a1 >> a2;

            if (!a1.empty())
            {
                // Первый операнд
                uint8_t size1 = getOperandSize(a1);
                pc += 1 + 1 + size1;  // type(1) + size(1) + value(size1)
                
                if (!a2.empty())
                {
                    // Второй операнд
                    uint8_t size2 = getOperandSize(a2);
                    pc += 1 + 1 + size2;  // type(1) + size(1) + value(size2)
                }
            }
        }
    }


    ss.clear();
    ss.seekg(0);

    auto &opcodeMap = decoder.getFactory().OpcodeMap;

    std::cout << "[Assembler] Assembling with " << opcodeMap.size() << " opcodes" << std::endl;
    std::cout << "[Assembler] Data variables found: " << dataVariables.size() << std::endl;
    for (const auto &[name, addr] : dataVariables)
    {
        std::cout << "  " << name << " -> " << addr << " (size: " << (int)dataSizes[name] << ")" << std::endl;
    }
    std::cout << "[Assembler] Code labels found: " << codeLabels.size() << std::endl;

    auto emitOperand = [&](TypeData type, uint8_t size, uint64_t value)
    {
        std::cout << "[Assembler] emitOperand: type=" << static_cast<int>(type)
                  << ", size=" << (int)size << ", value=" << value << std::endl;

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

        if (line.back() == ':')
            continue;

        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ls(line);

        std::string cmd;
        ls >> cmd;

        if (!opcodeMap.count(cmd))
        {
            std::cout << "[Assembler] ERROR: Unknown opcode '" << cmd << "'" << std::endl;
            throw std::runtime_error("Unknown opcode: " + cmd);
        }

        uint8_t opcode = opcodeMap.at(cmd);
        out.programBytes.push_back(opcode);

        std::cout << "\n[Assembler] Command: " << cmd << " (opcode=" << (int)opcode << ")" << std::endl;

        // -------- DATA DECLARATION --------
        if (cmd == "DB" || cmd == "DW" || cmd == "DD" || cmd == "DQ")
        {
            std::string name;
            uint64_t value;
            ls >> name >> value;

            uint8_t size = (cmd == "DB") ? 1 : (cmd == "DW") ? 2
                                           : (cmd == "DD")   ? 4
                                                             : 8;

            out.programBytes.push_back(2);
            
            uint64_t dataOffset = dataVariables[name];
            emitOperand(TypeData::Address, 8, dataOffset); 
            emitOperand(TypeData::Data, size, value);

            continue;
        }

        std::string a1, a2;
        ls >> a1 >> a2;

        std::cout << "[Assembler] Operands: a1='" << a1 << "', a2='" << a2 << "'" << std::endl;

        // -------- JUMP / THREAD --------
        if (cmd == "JMP" || cmd == "THRI" || cmd == "THRE")
        {
            if (!codeLabels.count(a1))
            {
                std::cout << "[Assembler] ERROR: Unknown label '" << a1 << "'" << std::endl;
                throw std::runtime_error("Unknown label: " + a1);
            }

            out.programBytes.push_back(1);                     
            emitOperand(TypeData::Address, 8, codeLabels[a1]);
            continue;
        }

        // -------- UNARY --------
        if (a2.empty())
        {
            out.programBytes.push_back(1); // operand_count = 1

            if (isRegister(a1))
            {
                emitOperand(TypeData::Register, 4, parseRegister(a1));
            }
            else if (isNumber(a1))
            {
                emitOperand(TypeData::Data, 4, std::stoull(a1));
            }
            else if (codeLabels.count(a1))
            {
                emitOperand(TypeData::Address, 8, codeLabels[a1]);
            }
            else if (dataVariables.count(a1))
            {
                emitOperand(TypeData::Address, 8, dataVariables[a1]);
            }
            else
            {
                std::cout << "[Assembler] ERROR: Invalid unary operand '" << a1 << "'" << std::endl;
                throw std::runtime_error("Invalid operand: " + a1);
            }
            continue;
        }

        // -------- BINARY --------
        out.programBytes.push_back(2); // operand_count = 2

        auto emitAuto = [&](const std::string &s)
        {
            std::cout << "[Assembler] Processing operand: '" << s << "'" << std::endl;

            if (isRegister(s))
            {
                std::cout << "  → Type: Register" << std::endl;
                emitOperand(TypeData::Register, 4, parseRegister(s));
            }
            else if (isNumber(s))
            {
                std::cout << "  → Type: Data (immediate)" << std::endl;
                emitOperand(TypeData::Data, 4, std::stoull(s));
            }
            else if (codeLabels.count(s))
            {
                std::cout << "  → Type: Address (code label at " << codeLabels[s] << ")" << std::endl;
                emitOperand(TypeData::Address, 8, codeLabels[s]);
            }
            else if (dataVariables.count(s))
            {
                std::cout << "  → Type: Address (data variable at offset " << dataVariables[s]
                        << ", size " << (int)dataSizes[s] << ")" << std::endl;
                
                // УПАКОВКА: старшие 56 бит - offset, младшие 8 бит - dataSize
                uint64_t packed = (dataVariables[s] << 8) | dataSizes[s];
                
                // Адрес всегда 8 байт
                emitOperand(TypeData::Address, 8, packed);
            }
            else
            {
                std::cout << "[Assembler] ERROR: Unknown operand '" << s << "'" << std::endl;
                std::cout << "  Known data variables: ";
                for (const auto &[name, _] : dataVariables)
                    std::cout << name << " ";
                std::cout << std::endl;
                std::cout << "  Known code labels: ";
                for (const auto &[name, _] : codeLabels)
                    std::cout << name << " ";
                std::cout << std::endl;

                throw std::runtime_error("Unknown operand: " + s);
            }
        };

        emitAuto(a1);
        emitAuto(a2);
    }

    out.dataVariables = dataVariables;
    out.label = codeLabels;
    out.dataSize = dataOffset;

    std::cout << "[Assembler] Assembly complete. Program size: " << out.programBytes.size()
              << " bytes, Data size: " << dataOffset << " bytes" << std::endl;

    return out;
}