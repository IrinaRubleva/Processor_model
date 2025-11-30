#pragma once
#include <cstddef>
#include "vector/include/Vector.hpp"


template <typename T>
class GeneralRegisters{
    Vector<int> vec{};
public:
    T read(size_t idx) const;
    void write(size_t idx, T val);
};