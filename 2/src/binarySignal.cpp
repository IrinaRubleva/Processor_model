#include "binarySignal.h"
#include "signalState.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <limits>
#include <cstddef>
#include <numeric>

BinarySignal::BinarySignal(const std::string &str) : signal(nullptr), size(0), capacity(0)
{
    try
    {
        if (str.empty())
            return;

        char currlvl = str[0];
        size_t duration = 1;

        for (size_t i = 1; i < str.length(); i++)
        {
            if (str[i] != '0' && str[i] != '1')
            {
                throw std::invalid_argument("Signal must be 0 or 1!");
            }

            if (str[i] == currlvl)
            {
                duration++;
            }
            else
            {
                bool level = (currlvl == '1');
                push_back(SignalState(level, duration));
                currlvl = str[i];
                duration = 1;
            }
        }
        bool final_level = (currlvl == '1');
        push_back(SignalState(final_level, duration));
    }
    catch (...)
    {
        delete[] signal;
        throw;
    }
}

BinarySignal::BinarySignal(bool lvl, std::size_t dur)
{
    try
    {
        if (dur == 0)
        {
            throw std::invalid_argument("Duration shoud me positive!");
        }
        push_back(SignalState(lvl, dur));
    }
    catch (...)
    {
        delete[] signal;
        throw;
    }
}

BinarySignal &BinarySignal::mergeStates()
{
    if (size <= 1)
    {
        return *this;
    }

    BinarySignal merged;
    merged.push_back(signal[0]);

    for (size_t i = 1; i < size; ++i)
    {
        if (signal[i].getLevel() == merged.back().getLevel())
        {
            size_t newDuration = merged.back().getDuration() + signal[i].getDuration();
            merged.back().setDuration(newDuration);
        }
        else
        {
            merged.push_back(signal[i]);
        }
    }

    *this = std::move(merged);
    return *this;
}

BinarySignal &BinarySignal::operator=(const BinarySignal &other)
{
    if (this != &other)
    {
        BinarySignal temp(other);
        swap(*this, temp);
    }
    return *this;
}
BinarySignal &BinarySignal::operator=(BinarySignal &&other) noexcept
{
    if (this != &other)
    {
        swap(*this, other);
    }
    return *this;
}

BinarySignal &BinarySignal::operator+=(const BinarySignal &other)
{
    for (std::size_t i = 0; i < other.size; i++)
    {
        push_back(other.signal[i]);
    }
    return *this;
}
BinarySignal BinarySignal::operator*(std::size_t n) const
{
    if (n == 0)
        return BinarySignal();

    BinarySignal res;
    for (std::size_t i = 0; i < n; i++)
    {
        res += *this;
    }
    res.mergeStates();
    return res;
}

bool BinarySignal::operator[](std::size_t time) const
{
    if (size == 0)
        return false;
    if (time >= getTotalDuration())
        return back().getLevel();

    std::size_t currTime = 0;
    for (std::size_t i = 0; i < size; i++)
    {
        std::size_t stateDuration = signal[i].getDuration();
        if (time < currTime + stateDuration)
        {
            return signal[i].getLevel();
        }
        currTime += stateDuration;
    }
    return false;
}

BinarySignal::BinarySignal(BinarySignal &&other) noexcept
    : signal(nullptr), size(0), capacity(0)
{
    swap(*this, other);
}

BinarySignal::BinarySignal(const BinarySignal &other)
    : signal(nullptr), size(0), capacity(0)
{

    if (other.size > 0)
    {
        signal = new SignalState[other.capacity];
        for (size_t i = 0; i < other.size; ++i)
        {
            signal[i] = other.signal[i];
        }
        size = other.size;
        capacity = other.capacity;
    }
}

BinarySignal &BinarySignal::inversionSignal()
{
    for (size_t i = 0; i < size; ++i)
    {
        signal[i].inversionState();
    }
    return *this;
}

size_t BinarySignal::getTotalDuration() const
{
    return std::accumulate(signal, signal + size, std::size_t(0),
                           [](std::size_t total, const SignalState &state)
                           {
                               return total + state.getDuration();
                           });
}

std::pair<size_t, size_t> BinarySignal::findIndexAtTime(size_t time) const
{
    if (size == 0)
    {
        throw std::invalid_argument("Signal is empty");
    }
    size_t currtime = 0;
    for (size_t i = 0; i < size; i++)
    {
        size_t stateDur = signal[i].getDuration();
        if (time <= currtime + stateDur)
        {
            return std::make_pair(currtime, i);
        }
        currtime += stateDur;
    }
    throw std::out_of_range("Time is biger than signal duration");
}

void BinarySignal::insertSignal(size_t time, const BinarySignal &other)
{
    if (other.size == 0)
        return;

    size_t totalDur = getTotalDuration();
    if (time >= totalDur)
    {
        for (size_t i = 0; i < other.size; ++i)
        {
            push_back(other.signal[i]);
        }
        mergeStates();
        return;
    }

    auto [startTime, index] = findIndexAtTime(time);
    size_t offset_left = time - startTime;
    size_t offset_right = signal[index].getDuration() - offset_left;

    BinarySignal newSignal;

    for (size_t i = 0; i < index; ++i)
    {
        newSignal.push_back(signal[i]);
    }

    if (offset_left > 0)
    {
        newSignal.push_back(SignalState(signal[index].getLevel(), offset_left));
    }

    for (size_t i = 0; i < other.size; ++i)
    {
        newSignal.push_back(other.signal[i]);
    }

    if (offset_right > 0)
    {
        newSignal.push_back(SignalState(signal[index].getLevel(), offset_right));
    }

    for (size_t i = index + 1; i < size; ++i)
    {
        newSignal.push_back(signal[i]);
    }

    *this = static_cast<BinarySignal &&>(newSignal);
    mergeStates();
}

void BinarySignal::deleteSignal(size_t startTime, size_t delDur)
{
    if (delDur == 0 || size == 0)
        return;

    size_t totalDur = getTotalDuration();
    if (startTime >= totalDur)
        throw std::out_of_range("Start time is bigger than signal duration");

    size_t endTime = startTime + delDur;
    if (endTime >= totalDur)
    {
        endTime = totalDur;
        delDur = totalDur - startTime;
    }

    auto [startStateTime, startInd] = findIndexAtTime(startTime);
    auto [endStateTime, endInd] = findIndexAtTime(endTime);

    BinarySignal newSignal;

    for (size_t i = 0; i < startInd; ++i)
    {
        newSignal.push_back(signal[i]);
    }

    size_t offset_left = startTime - startStateTime;
    size_t offset_right = signal[endInd].getDuration() - (endTime - endStateTime);

    if (offset_left > 0)
    {
        newSignal.push_back(SignalState(signal[startInd].getLevel(), offset_left));
    }

    if (offset_right > 0)
    {
        newSignal.push_back(SignalState(signal[endInd].getLevel(), offset_right));
    }

    for (size_t i = endInd + 1; i < size; ++i)
    {
        newSignal.push_back(signal[i]);
    }

    *this = std::move(newSignal);
    mergeStates();
}

std::string BinarySignal::formatSignal() const
{
    std::string res;
    if (size == 0)
        return res;
    for (size_t i = 0; i < size; ++i)
    {
        res += signal[i].formatState();
        if (signal[i].getLevel() == false)
        {
            res += "/";
        }
        else
        {
            res += "\\";
        }
    }
    return res;
}

void BinarySignal::outputSignal(std::ostream &os) const
{
    for (size_t i = 0; i < size; ++i)
    {
        signal[i].outputState(os);
    }
}

void BinarySignal::inputSignal(std::istream &is)
{
    const static char stopSimbol = '.';
    std::string inputStr;
    while (is.good())
    {
        char c = is.get();
        if (c == stopSimbol || c == '\n' || c == EOF)
        {
            break;
        }
        if (c != ' ' && c != '\t')
        {
            inputStr += c;
        }
    }
    if (!inputStr.empty())
    {
        *this = BinarySignal(inputStr);
    }
}

void BinarySignal::expander(std::size_t new_capacity)
{
    if (new_capacity <= capacity && capacity != 0)
        return;
    if (new_capacity == 0)
        new_capacity = 1;

    SignalState *new_signal = new SignalState[new_capacity];
    std::move(signal, signal + size, new_signal);
    delete[] signal;
    signal = new_signal;
    capacity = new_capacity;
}

void BinarySignal::push_back(const SignalState &state)
{
    if (size >= capacity)
    {
        expander(capacity * 2);
    }
    signal[size++] = state;
}

SignalState &BinarySignal::back()
{
    if (size == 0)
        throw std::invalid_argument("Size is zero");
    return signal[size - 1];
}

const SignalState &BinarySignal::back() const
{
    if (size == 0)
        throw std::invalid_argument("Size is zero");
    return signal[size - 1];
}

std::ostream &operator<<(std::ostream &os, const BinarySignal &signal)
{
    signal.outputSignal(os);
    return os;
}

std::istream &operator>>(std::istream &is, BinarySignal &signal)
{
    try {
        signal.inputSignal(is);
    }
    catch (const std::exception& e) {
        is.setstate(std::ios::failbit);
    }
    return is;
}
