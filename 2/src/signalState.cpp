#include "signalState.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <cstddef>


SignalState::SignalState(const std::string &str){
    if(str.empty()){
        level = false;
        duration = 0;
        return;
    }
    char first = str[0];
    if(first != '0' && first != '1'){
        throw std::invalid_argument("Signal must be 0 or 1!");
    }
    
    level = (first == '1');
    duration = 0;
    
    for(size_t i = 0; i < str.length() && str[i] == first; i++) {
        duration++;
    }
}
    
bool SignalState:: getLevel() const {return level;}

size_t SignalState:: getDuration() const {return duration;}

void SignalState:: setLevel(bool lvl){
    level = lvl;
}

void SignalState:: setLevel(char lvl){
    if(lvl == '1') {level = true;}
    else if (lvl == '0') {level = false;}
    else {throw std::invalid_argument("Signal must be 0 or 1!");}
}

void SignalState:: setLevelInt(int levelInt) {
    if (levelInt == 1) {level = true;} 
    else if (levelInt == 0) {level = false;} 
    else {throw std::invalid_argument("Signal must be 0 or 1!");}
}

void SignalState:: setDuration(size_t newDur) {
    validDuration(newDur);
    duration = newDur;
}

void SignalState:: setState(bool newLevel, size_t newDuration) {
    validDuration(newDuration);
    level = newLevel;
    duration = newDuration;
}

void SignalState:: inputState(std::istream & is){
    char lvl;
    size_t dur;
    //std::cout << "Enter level of signal (0 or 1):" << std::flush;
    is >> lvl;
    if (lvl != '0' && lvl != '1'){
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::invalid_argument("Signal must be 0 or 1!");
    }

    //std::cout << "Enter duration (natural number): " << std::flush;
    is >> dur;
    if (is.fail()){
        is.clear();
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::invalid_argument("Duration must be natural number!");
    }

     level = (lvl == '1'); 
    duration = dur;
}

void SignalState:: outputState(std::ostream &os) const {
    os << "SignalState: [level=" << (level ? "true" : "false") << "; duration=" << duration << "]\n";
}

SignalState & SignalState:: inversionState(){
    level = !level;
    return *this;
}

void SignalState::lengtheningState(size_t time){
    if (time > SIZE_MAX - duration){ throw std::overflow_error("Overflow!");}
    duration += time;
}

void SignalState::shorteningState(size_t time){
    if (duration - time < 0){ throw std::underflow_error("Underflow!");}
    duration -= time;
}

std::string SignalState::formatState() const{
    if (duration == 0) return "";
    //char symbol = level ? '‾' : '_';
    char symbol = level ? '-' : '_';
    return std::string(duration, symbol);
}

void SignalState::validDuration(size_t dur){
    if(dur <= 0) throw std::invalid_argument("Duration shoud be positive!");
}


std::ostream& operator<<(std::ostream& os, const SignalState & state){
    state.outputState(os);
    return os;
}

std::istream & operator>>(std::istream & is, SignalState & state){
    state.inputState(is);
    return is;
}


