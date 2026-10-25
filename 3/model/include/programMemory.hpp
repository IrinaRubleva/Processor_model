#pragma once
#include <vector>
#include <memory>
#include "device/memoryDevice.hpp"

#include <iomanip>
#include <iostream>




/*!
*@brief Программа памяти
*/
class ProgramMemory : public MultiByteMemoryDevice
{
public:
    explicit ProgramMemory(size_t size) : MultiByteMemoryDevice(size){}

    ~ProgramMemory() override = default;

    ProgramMemory(const ProgramMemory &) = delete;
    ProgramMemory &operator=(const ProgramMemory &) = delete;

    ProgramMemory(ProgramMemory &&other) noexcept = default;
    ProgramMemory &operator=(ProgramMemory &&other) noexcept = default;

    void clear()
    {
        for (size_t i = 0; i < getSize(); ++i)
        {
            write(i, 0);
        }
    }
    void loadProgram(const std::vector<uint8_t> &bytes)
    {
        clear();
        for (size_t i = 0; i < bytes.size(); ++i)
        {
            write(i, bytes[i]);
        }
    }

    void printState(size_t bytesPerLine = 16, size_t maxBytes = 0) const
    {
        if (maxBytes == 0 || maxBytes > getSize()) {
            maxBytes = getSize();
        }
        
        for (size_t addr = 0; addr < maxBytes; addr += bytesPerLine) {
           
            
            for (size_t i = 0; i < bytesPerLine; ++i) {
                size_t currentAddr = addr + i;
                if (currentAddr < maxBytes) {
                    std::cout << std::dec << std::setw(3) << std::setfill(' ') 
                             << static_cast<int>(read(currentAddr)) << " ";
                } else {
                    std::cout << "    ";
                }
            }
            
        
            for (size_t i = 0; i < bytesPerLine; ++i) {
                size_t currentAddr = addr + i;
                if (currentAddr < maxBytes) {
                    uint8_t byte = read(currentAddr);
                    if (byte >= 32 && byte <= 126) { 
                        std::cout << static_cast<char>(byte);
                    } else {
                        std::cout << ".";
                    }
                }
            }
            
           std::cout << std::endl;
        }
    }
};

