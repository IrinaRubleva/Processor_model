#include <iostream>
#include "dictionary.h"
#include <string>
#include <limits>
#include <cstring>


std::string readNameFile(){
    std::cout << "Enter name of file: " << std::flush;
    std::string fileName;
    std::cin >> fileName;
    return fileName;
}

unsigned int showMenuAndGetChoice() {
    std::cout << "Choose option:\n 1. Convert to XML\n 2. Convert from XML\n 3. Exit\n > " << std::endl;
    
    unsigned int option = 0;
    if (!(std::cin >> option)) {
        if (std::cin.eof()) {
            std::cout << "EOF!\n";
            return 4;
        }
        throw std::runtime_error("Invalid input! Enter the number.");
    }
    
    return option;
}