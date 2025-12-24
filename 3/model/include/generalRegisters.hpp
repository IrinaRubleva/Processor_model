#pragma once
#include <cstddef>
#include "vector/include/Vector.hpp"



class GeneralRegisters{
    Vector<uint32_t> vec{};
public:
    GeneralRegisters(): vec(16, 0) {}  

    explicit GeneralRegisters(size_t num_registers, uint32_t initial_value = 0): vec(num_registers, initial_value)
    {
        if (num_registers == 0) {
            throw std::invalid_argument("GeneralRegisters: number of registers must be > 0");
        }
    }
    
    
    GeneralRegisters(std::initializer_list<uint32_t> init_list): vec(init_list) 
    {
        if (vec.size() == 0) {
            throw std::invalid_argument("GeneralRegisters: must have at least one register");
        }
    }
    
   
    GeneralRegisters(const GeneralRegisters& other) = default;
    GeneralRegisters(GeneralRegisters&& other) noexcept = default;
    
    ~GeneralRegisters() = default;


    uint32_t read(size_t idx) const {
        if (idx >= vec.size()) {
            throw std::out_of_range("GeneralRegisters::read out of range");
        }
        return vec[idx];
    }
    
    void write(size_t idx, uint32_t val) {
        if (idx >= vec.size()) {
            throw std::out_of_range("GeneralRegisters::write - index out of range");
        }
        vec[idx] = val;
    }

    void reset() noexcept {
        for (size_t i = 0; i < vec.size(); ++i) {
            vec[i] = 0;
        }
    }
};