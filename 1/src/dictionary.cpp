#include <string>
#include "dictionary.h"
#include <cstring>
#include <stdexcept>  
#include <iostream>

char* toXml(const Dictionary& record, const char* recordName){
     if(!recordName){
        throw std::invalid_argument("recordName cannot be null");
    }
    std::string stringXml = "<" + std::string(recordName) + ">\n\t<word>" + record.word + "</word>\n\t<meaning>" + record.meaning + "</meaning>\n\t<emphasis>" + std::to_string(record.emphasis) + "</emphasis>\n</" + std::string(recordName) + ">";
    char* res = new char[stringXml.length() + 1];
    strncpy(res, stringXml.c_str(), stringXml.length() + 1);
    res[stringXml.length()] = '\0';
    return res;
}


char* toXml(const Dictionary& record, const char* recordName, size_t nameSize){
     if(!recordName){
        throw std::invalid_argument("recordName cannot be null");
    }
    std::string stringNameRecord(recordName, nameSize);
    std::string stringXml = "<" + stringNameRecord + ">\n\t<word>" + record.word + "</word>\n\t<meaning>" + record.meaning + "</meaning>\n\t<emphasis>" + std::to_string(record.emphasis) + "</emphasis>\n</" + std::string(recordName) + ">";
    char* res = new char[stringXml.length() + 1];
    std::strncpy(res, stringXml.c_str(), stringXml.length() + 1);
    res[stringXml.length()] = '\0';
    return res;
}

std::string toXml(const Dictionary& record, const std::string& recordName){
    return "<" + recordName + ">\n\t<word>" + record.word + "</word>\n\t<meaning>" + record.meaning + "</meaning>\n\t<emphasis>" + std::to_string(record.emphasis) + "</emphasis>\n</" + recordName + ">";
}



Dictionary fromXml(const char* xmlString, const char* recordName){
     if(!recordName){
        throw std::invalid_argument("recordName cannot be null");
    }
    if(!xmlString){
        throw std::invalid_argument("xmlString cannot be null");
    }
    std::string strXml(xmlString);
    std::string stringNameRecord(recordName);
    Dictionary dictionary = fromXml(strXml, stringNameRecord);
    return dictionary;
}


Dictionary fromXml(const char* xmlString, size_t xmlSize, const char* recordName, size_t nameSize){
     if(!recordName){
        throw std::invalid_argument("recordName cannot be null");
    }
    if(!xmlString){
        throw std::invalid_argument("xmlString cannot be null");
    }
    std::string stringNameRecord(recordName, nameSize);
    std::string strXml(xmlString);
    Dictionary dictionary = fromXml(strXml, stringNameRecord);
    return dictionary;
}


Dictionary fromXml(const std::string& xmlString, const std::string& recordName){
    Dictionary dictionary;
    try {
        size_t indexWordStart = xmlString.find("<word>");
        if (indexWordStart == std::string::npos) {
            throw std::runtime_error("Tag <word> not found");
        }
        indexWordStart += 6;
        size_t indexWordEnd = xmlString.find("</word>", indexWordStart);
        if (indexWordEnd == std::string::npos) {
            throw std::runtime_error("Tag </word> not found");
        }
        dictionary.word = xmlString.substr(indexWordStart, indexWordEnd - indexWordStart);

        
        size_t indexMeaningStart = xmlString.find("<meaning>");
        if (indexMeaningStart == std::string::npos) {
            throw std::runtime_error("Tag <meaning> not found");
        }
        indexMeaningStart += 9;
        size_t indexMeaningEnd = xmlString.find("</meaning>", indexMeaningStart);
        if (indexMeaningEnd == std::string::npos) {
            throw std::runtime_error("Tag </meaning> not found");
        }
        dictionary.meaning = xmlString.substr(indexMeaningStart, indexMeaningEnd - indexMeaningStart);

        size_t indexEmphasisStart = xmlString.find("<emphasis>");
        if (indexEmphasisStart == std::string::npos) {
            throw std::runtime_error("Tag <emphasis> not found");
        }
        indexEmphasisStart += 10;

        size_t indexEmphasisEnd = xmlString.find("</emphasis>", indexEmphasisStart);
        if (indexEmphasisEnd == std::string::npos) {
            throw std::runtime_error("Tag </emphasis> not found");
        }

        std::string emphasisStr = xmlString.substr(indexEmphasisStart, indexEmphasisEnd - indexEmphasisStart);

        if (emphasisStr.empty()) {
            dictionary.emphasis = 0; 
        } else {
            try {
                dictionary.emphasis = std::stoul(emphasisStr);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Could not convert emphasis '" << emphasisStr << "' to number. Using default value 0." << std::endl;
                dictionary.emphasis = 0;
            }
        }
    }catch (const std::exception& e) {
        std::cerr << "XML parsing error: " << e.what() << std::endl;
    }
    return dictionary;
}

void freeXml(const char* xml){
    delete[] xml;
}