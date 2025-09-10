#include <iostream>
#include "dictionary.h"
#include <string>
#include <limits>
#include <cstring>


void convertToXml(const Dictionary& dictionary) {
    try {
        std::cout << "~~Converting to XML~~\n";

        std::cout << "1. Record type: const char*\n";
        const char* recordName = "record";
        const char* resTo1 = toXml(dictionary, recordName);
        std::cout << resTo1 << std::endl;

        std::cout << "2. Record type: const char* + size_t\n";
        size_t length = strlen(recordName);
        const char* resTo2 = toXml(dictionary, recordName, length);
        std::cout << resTo2 << std::endl;

        std::cout << "3. Record type: std::string\n";
        std::string str(recordName);
        std::string resTo3 = toXml(dictionary, str);
        std::cout << resTo3 << std::endl;

        freeXml(resTo1);
        freeXml(resTo2);
    }
    catch (const std::exception& e) {
        std::cerr << "Error converting to XML: " << e.what() << std::endl;
        throw;
    }
}

void convertFromXml(const std::string& xmlString) {
    try {
        const char* recordName = "record";
        std::string str(recordName);
        const char* c_xmlString = xmlString.c_str();

        std::cout << "~~Converting from XML~~\n";
        
        std::cout << "1. Record type: const char*\n";
        Dictionary record1 = fromXml(c_xmlString, recordName);
        std::cout << "Word: " << record1.word << "\n" << "Meaning: " << record1.meaning << "\n" << "Emphasis: " << std::to_string(record1.emphasis) << "\n";
        
        std::cout << "2. Record type: const char* + size_t\n";
        Dictionary record2 = fromXml(c_xmlString, strlen(c_xmlString), recordName, strlen(recordName));
        std::cout << "Word: " << record2.word << "\n" << "Meaning: " << record2.meaning << "\n" << "Emphasis: " << std::to_string(record2.emphasis) << "\n";
        
        std::cout << "3. Record type: std::string\n";
        Dictionary record3 = fromXml(xmlString, str);
        std::cout << "Word: " << record3.word << "\n" << "Meaning: " << record3.meaning << "\n" << "Emphasis: " << std::to_string(record3.emphasis) << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error converting from XML: " << e.what() << std::endl;
        throw;
    }
}

Dictionary inputDictionaryRecord() {
    Dictionary dict;
    std::string word, meaning;
    size_t emphasis;
    
    std::cout << "Enter word (max 20 symbol): ";
    std::cin >> word;
    if (std::cin.eof()) {
        throw std::runtime_error("EOF encountered");
    }
    
    if (word.length() > 20) {
        word = word.substr(0, 20);
        std::cout << "Your word has been shortened to " << word << std::endl;
    }
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "Enter meaning (no limits): ";
    std::getline(std::cin, meaning);
    if (std::cin.eof()) {
        throw std::runtime_error("EOF encountered");
    }
    
    std::cout << "Enter emphasis (number): ";
    while (!(std::cin >> emphasis) || emphasis == 0) {
        if (std::cin.eof()) {
            throw std::runtime_error("EOF encountered");
        }
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw std::runtime_error("Invalid input! Enter the number.");
        }
        std::cout << "Try again" << std::endl;
    }
    
    dict.word = word;
    dict.meaning = meaning;
    dict.emphasis = emphasis;
    
    return dict;
}

std::string getXmlStringFromUser() {
    std::cout << "Enter the XML line: \n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::string xmlString;
    std::getline(std::cin, xmlString);
    if (std::cin.eof()) {
        throw std::runtime_error("EOF encountered");
    }
    
    return xmlString;
}

unsigned int showMenuAndGetChoice() {
    std::cout << "Choose option:\n 1. Enter record\n 2. Convert to XML\n 3. Convert from XML\n 4. Exit\n > " << std::endl;
    
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