#pragma once
#include <iostream>
#include <string>
#include <stdexcept>
#include <limits>
#include <cstddef>

class SignalState{
public:
    SignalState(): level(false), duration(1) {}  
    SignalState(bool lvl, std::size_t dur): level(lvl), duration(dur) {validDuration(dur);}
    SignalState(const std::string &str);
     
    bool getLevel() const;
    std::size_t getDuration() const;

    void setLevel(bool lvl);
    void setLevel(char lvl);
    void setLevelInt(int levelInt);
    void setDuration(size_t newDur);
    void setState(bool newLevel, size_t newDuration);

    void inputState(std::istream & is);
    void outputState(std::ostream &os) const ;

    SignalState & inversionState();

    void lengtheningState(size_t time);
    void shorteningState(size_t time);

    std::string formatState() const;

    friend std::ostream& operator<<(std::ostream& os, const SignalState & state);

    friend std::istream & operator>>(std::istream & is, SignalState & state);

private: 
    bool level;
    std::size_t duration;
    void validDuration(std::size_t dur);
};
