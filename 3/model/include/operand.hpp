#pragma once

/**
 * @brief  Enum для обозначения типа данных в OperandDescription 
 * @see OperandDescription: значение или адрес в шине, где хранится значенние, представленное в двоичной системе
 */
enum class TypeData {
    Data, ///< Непосредственное значение
    Address  ///< Адрес в шине, где хранится значение
};


/**
 * @class OperandDescription
 * @brief Описание операнда инструкции
 * 
 * Класс описывает операнд, который может быть либо непосредственным значением,
 * либо адресом в памяти, доступным через шину.
 */
class OperandDescriptor{
private:
    TypeData type;  ///< Тип данных (значение или адрес)
    uint8_t size;   ///< Размер данных в байтах (1, 2, 4, 8)
    uint64_t value; ///< Значение или адрес памяти в битах (например, 0x)

public:
    uint64_t getRawValue(Bus& bus){
        if (type == TypeData::Data) return value;

        uint64_t res = 0;
        for (uint8_t i = 0; i < size; ++i) {
            res |= (uint64_t)bus.read(value + i) << (i * 8);
        }
        return res;
    }

    uint8_t getSize() const {return size;};

    TypeData getType() const {return type;};

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
        uint64_t mask = (1ULL << (size*8)) - 1;
        return (uint64_t)val & mask;
    }
};
