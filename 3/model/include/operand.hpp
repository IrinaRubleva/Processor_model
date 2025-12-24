#pragma once
#include "device/bus.hpp"
#include <cstdint>
#include <iostream>





/*!
*@brief Описатель операнда. Входит в структуру команда
*/


enum class TypeData {
    Data = 0,     ///< Непосредственное значение
    Address = 1,  ///< Адрес в шине, где хранится значение
    Register = 2  ///< Регистр
};

class OperandDescriptor {
private:
    TypeData type;  ///< Тип данных (значение или адрес)
    uint8_t size;   ///< Размер данных в байтах (1, 2, 4, 8)
    uint64_t value; ///< Значение или упакованные данные

public:
    OperandDescriptor() = default;
    OperandDescriptor(TypeData type, uint8_t size, uint64_t val): type(type), size(size), value(val) {}
    
    uint64_t getValue() const {return value;}
    uint8_t getSize() const { return size; }
    TypeData getType() const { return type; }
    void setValue(uint64_t val) {value = val;}

    uint64_t getRawValue(Bus& bus) {
        if (type == TypeData::Data) return value;
        
        else if (type == TypeData::Register) {
            uint64_t gprBase = bus.getDeviceAddress("GPR");
            uint64_t res = 0;
            for (uint8_t i = 0; i < size; ++i) {
                res |= (uint64_t)bus.read(gprBase + value * size + i) << (i * 8);
            }
            return res;
        }

        else if (type == TypeData::Address) {
            // РАСПАКОВКА: старшие 56 бит - offset, младшие 8 бит - dataSize
            uint64_t offset = value >> 8;     // offset в DataMemory
            uint8_t dataSize = value & 0xFF;  // размер переменной (1, 2, 4, 8)
            
            uint64_t dataMemBase = bus.getDeviceAddress("DataMemory");
            uint64_t absoluteAddr = dataMemBase + offset;
            //std::cout << "[OperandDescriptor] DataMemory base address: " << dataMemBase << " (should be 2056)" << std::endl;
            
            //std::cout << "[OperandDescriptor::getRawValue] Address type: " << "offset=" << offset << ", dataSize=" << (int)dataSize << ", reading from addr=" << absoluteAddr << std::endl;
            
            uint64_t res = 0;
            for (uint8_t i = 0; i < dataSize; ++i) {
                uint8_t byte = bus.read(absoluteAddr + i);
                res |= (uint64_t)byte << (i * 8);
            }
            
            //std::cout << "[OperandDescriptor::getRawValue] Read value: " << res << std::endl;
            return res;
        }
        
        throw std::runtime_error("Unknown operand type");
        return 0;
    }

    void setRawValue(Bus& bus, uint64_t newValue) {
        //std::cout << "[OperandDescriptor::setRawValue] type=" << (int)type  << ", current value=" << value << ", newValue=" << newValue << std::endl;
    
        if (type == TypeData::Data) {
            
            value = newValue;
        }
        else if (type == TypeData::Register) {
            uint64_t gprBase = bus.getDeviceAddress("GPR");
            
            
            uint64_t registerIndex = value;
            uint64_t regAddr = gprBase + registerIndex * size;
            
            
            for (uint8_t i = 0; i < size; ++i) {
                uint8_t byteValue = (newValue >> (i * 8)) & 0xFF;
                bus.write(regAddr + i, byteValue);
            }
        }
        else if (type == TypeData::Address) {
            
            uint64_t offset = value >> 8;     // offset в DataMemory
            uint8_t dataSize = value & 0xFF;  // размер переменной (1, 2, 4, 8)
            
            uint64_t dataMemBase = bus.getDeviceAddress("DataMemory");
            uint64_t absoluteAddr = dataMemBase + offset;
            
            
            
            for (uint8_t i = 0; i < dataSize; ++i) {
                uint8_t byteValue = (newValue >> (i * 8)) & 0xFF;
                bus.write(absoluteAddr + i, byteValue);
            }
        }
        else {
            throw std::runtime_error("Unknown operand type for writing");
        }
    }

    int64_t signExtend(uint64_t value, uint8_t size) {
        uint8_t bits = size * 8;
        uint64_t signBit = 1ULL << (bits - 1);

        if (value & signBit) {
            uint64_t mask = (~0ULL) << bits;
            return (int64_t)(value | mask);
        }
        return (int64_t)value;
    }

    uint64_t packToN(int64_t val, uint8_t size) {
        if (size == 8) return (uint64_t)val;
        uint64_t mask = (1ULL << (size * 8)) - 1;
        return (uint64_t)val & mask;
    }
};