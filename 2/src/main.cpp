#include <iostream>
#include "binarySignal.h"
#include "signalState.h"
#include <string>
#include <stdexcept>
#include <algorithm>


int main(){
    SignalState state;
    BinarySignal signal;
    std::string opt;
    std::string arr[] = {"i", "is", "ls", "shs", "fs", "f", "add", "ins", "ds", "stop"};

    std::cout << "Enter value for state(), then for signal\n For example: 1111 <level=true, duration=11>\n 1010 <signal>\n";
    std::cin >> state;
    if (std::cin.eof()) return 0;
    std::cin >> signal; 
    if (std::cin.eof()) return 0; 
    while (std::cin.good())
    {

        std::cout << "------OPTIONS------";
        std::cout << "for inversion state - is\n for inversion signal - i\n for lengthening state - ls\n for shortening state - shs\n";
        std::cout << "for formatting state - fs\n  for formatting signal- f\n for adding signal to signal - add\n";
        std::cout << "for insert signal to signal - ins\n for signal duplication - ds\n";
        std::cout << "\n";
        std::cin >> opt;
        if (!(std::find(std::begin(arr), std::end(arr), opt) != std::end(arr))){
            throw std::invalid_argument("Invalid argument!");
        }
        
        if(opt == "i"){std::cout << state.inversionState();}
        if(opt == "is"){std::cout << signal.inversionSignal();}
        if(opt == "ls"){
            size_t time;
            std::cout << "Enter the time of lengthening:\n";
            std:: cin >> time;
            state.lengtheningState(time);
            std::cout << state << "\n";
        }
        if(opt == "shs"){
            size_t time;
            std::cout << "Enter the time of shorthening:\n";
            std:: cin >> time;
            state.shorteningState(time);
            std::cout << state << "\n";
        }
        if(opt == "fs"){std::cout << state.formatState();}
        if(opt == "f"){std::cout << signal.formatSignal();}
        if(opt == "add"){
            SignalState bs;
            std::cout << "Enter the complementary signal\n";
            std:: cin >> bs;
            signal += bs;
            std::cout << signal << "\n";
        }
        if(opt == "ins"){
            SignalState bs;
            size_t time;
            std::cout << "Enter the complementary signal and the insertion time:\n";
            std:: cin >> bs >> time;
            signal.insertSignal(time, bs);
            std::cout << signal << "\n";
        }
        if(opt == "ds"){
            size_t n;
            std::cout << "Enter the number of repetitions:\n";
            std:: cin >> n;
            signal = signal*n;
            std::cout << signal << "\n";
        }
        if (opt == "stop"){
            return 0;
        }
    }
    std::cout << "Program finished (EOF reached)." << std::endl;
    return 0;
} 