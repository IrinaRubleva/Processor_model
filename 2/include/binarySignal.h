#pragma once
#include "signalState.h"
#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <cstddef> 

class BinarySignal{
public:
    BinarySignal(): signal(nullptr), capacity(0), size(0){}
    BinarySignal(const SignalState& sgn): signal(nullptr), capacity(0), size(0) { push_back(sgn);}
    BinarySignal(const std::string& str);
    BinarySignal(bool lvl, std::size_t dur);

    
    BinarySignal& operator=(const BinarySignal& other);
    BinarySignal& operator+=(const BinarySignal& other);
    
    BinarySignal operator*(std::size_t n) const;
    friend BinarySignal operator*(std::size_t n, const BinarySignal& bs) {
        return bs * n;
    }

    friend std::ostream& operator<<(std::ostream& os, const BinarySignal& signal);;
    friend std::istream & operator>>(std::istream & is, BinarySignal & signal);
    
    bool operator[](std::size_t time) const;
    
    BinarySignal &inversionSignal();

    std::size_t getTotalDuration() const;
    std::pair<std::size_t, std::size_t> findIndexAtTime(std::size_t time) const;

    BinarySignal & mergeStates();

    void insertSignal(std::size_t startTime, const BinarySignal& other);
    void deleteSignal(std::size_t startTime, std::size_t delDur);

    std::string formatSignal() const;

    void inputSignal(std::istream & is);
    void outputSignal(std::ostream &os) const ;    

    void push_back(const SignalState & state);

    SignalState& back();

    const SignalState& back() const;

    BinarySignal(const BinarySignal & other);
    BinarySignal(BinarySignal &&other);
    BinarySignal& operator=(BinarySignal&& other);
    ~BinarySignal() { delete[] signal;}


private:
    SignalState * signal;
    std::size_t capacity;
    std::size_t size;
    void expander(std::size_t new_capacity);
};
