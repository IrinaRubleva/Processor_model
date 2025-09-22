#include <string>
#include "dictionary.h"
#include "dialog.h"
#include <cstring>
#include <stdexcept>  
#include <iostream>
#include <vector>
#include <fstream>
#include <format>


using namespace std::string_literals;

std::string xmlLine(const Dictionary& record, const std::string& recordName){
    return std::format("<{0}>\n\t<word>{1}</word>\n\t<meaning>{2}</meaning>\n\t<emphasis>{3}</emphasis>\n</{0}>", recordName, record.word, record.meaning, std::to_string(record.emphasis));
}

char* toXml(const Dictionary& record, const char* recordName){
     if(!recordName){
        throw std::invalid_argument("recordName cannot be null");
    }
    std::string stringXml = xmlLine(record, std::string(recordName));
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
    std::string stringXml = xmlLine(record, std::string(recordName));
    char* res = new char[stringXml.length() + 1];
    std::strncpy(res, stringXml.c_str(), stringXml.length() + 1);
    res[stringXml.length()] = '\0';
    return res;
}

std::string toXml(const Dictionary& record, const std::string& recordName){
    return xmlLine(record, recordName);
}

Dictionary fromXml( const char* xmlString, const char* recordName){
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


std::string findWord(std::string wordStart, std::string wordFinish, const std::string& xmlString) noexcept {
    size_t indexWordStart = xmlString.find(wordStart);
    if (indexWordStart == std::string::npos) {
        return "";
    }
    indexWordStart += wordStart.length();
    size_t indexWordEnd = xmlString.find(wordFinish, indexWordStart);
    if (indexWordEnd == std::string::npos) {
        return "";
    }
    if (indexWordEnd > indexWordStart){
        return xmlString.substr(indexWordStart, indexWordEnd - indexWordStart);
    }
    return "";
}

Dictionary fromXml(const std::string& xmlString, const std::string& recordName){
    Dictionary dictionary;
    try {
        dictionary.word = findWord("<word>", "</word>", xmlString);
        dictionary.meaning = findWord("<meaning>", "</meaning>", xmlString);

        std::string emphasisStr = findWord("<emphasis>", "</emphasis>", xmlString);
        if (emphasisStr.empty()) {
            dictionary.emphasis = 0; 
        } else {
            dictionary.emphasis = std::stoul(emphasisStr);
        }

    }catch (const std::exception& e) {
        std::cerr << "XML parsing error: " << e.what() << std::endl;
    }
    return dictionary;
}


struct XmlConverter1{
    char *operator()(const Dictionary& dict, const char* recordName){
        return toXml(dict, recordName);
    }
};

struct XmlConverter2{
    char *operator()(const Dictionary& dict, const char* recordName){
        return toXml(dict, recordName, strlen(recordName));
    }
};

struct XmlConverter3{
    std::string operator()(const Dictionary& dict, const char* recordName){
        return toXml(dict, std::string(recordName));
    }
};

struct DictConverter1{
    Dictionary operator()(const char* xmlString, const char* recordName){
        return fromXml(xmlString, recordName);
    }
};

struct DictConverter2{
    Dictionary operator()(const char* xmlString, const char* recordName){
        return fromXml(xmlString, strlen(xmlString), recordName, strlen(recordName));
    }
};

struct DictConverter3{
    Dictionary operator()(const char* xmlString, const char* recordName){
        return fromXml(std::string(xmlString), std::string(recordName));
    }
};

template<typename Converter>
void writeToFile_Converter(const std::vector<Dictionary>& dictionary, const std::string& fileName, const char* recordName, Converter converter){
    std::ofstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create file: " + fileName);
    }

    file << "<records>\n";
    for(const auto& dict: dictionary){
        auto form = converter(dict, recordName);
        file << form << "\n";

        if constexpr (std::is_same_v<decltype(form), char*>) {
            delete[] form;
        }
    }
    file << "</records>\n";
    file.close();
}

template<typename Converter>
void writeFromFile_Converter(const std::vector<Dictionary>& dictionary, const std::string& fileName, const char* recordName, Converter converter) {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + fileName);
    }
    
    for (const auto& dict : dictionary) {
        file << dict.word << "\n" << dict.meaning << "\n" << dict.emphasis << "\n";
    }
    file.close();
}


void writeConverterFiles(const std::vector<Dictionary>& dictionary, 
                       const std::string& extension,
                       bool isXmlConversion) {
    std::string fileToName = readNameFile();
    const char* recordName = "record";
    
    std::string file1 = fileToName + "1" + extension;
    std::string file2 = fileToName + "2" + extension;
    std::string file3 = fileToName + "3" + extension;
    
    if (isXmlConversion) {
        writeToFile_Converter(dictionary, file1, recordName, XmlConverter1{});
        writeToFile_Converter(dictionary, file2, recordName, XmlConverter2{});
        writeToFile_Converter(dictionary, file3, recordName, XmlConverter3{});
    } else {
        writeFromFile_Converter(dictionary, file1, recordName, DictConverter1{});
        writeFromFile_Converter(dictionary, file2, recordName, DictConverter2{});
        writeFromFile_Converter(dictionary, file3, recordName, DictConverter3{});
    }
    
    std::cout << "Files created: " << file1 << ", " << file2 << ", " << file3 << std::endl;
}


void writeFile_ToXml(const std::vector<Dictionary>& dictionary) {
    writeConverterFiles(dictionary, ".xml", true);
}

void writeFile_FromXml(const std::vector<Dictionary>& dictionary) {
    writeConverterFiles(dictionary, ".txt", false);
}


std::string fileReader(){
    std::string fileName = readNameFile();
    std::ifstream file(fileName);
    if(!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + fileName);
    }

    std::string xmlContent;
    std::string line;
    while(std::getline(file, line)) {
        xmlContent += line + "\n";
    }
    file.close();
}

std::vector <Dictionary> readDictionaryFromFile(){
    std::vector<Dictionary> dictionary;
    std::string xmlContent = fileReader();
    size_t pos = 0;
    while (pos < fileContent.length()) {
        Dictionary dict;
        
        dict.word = findWord("word=\"", "\"", fileContent.substr(pos));
        if (dict.word.empty()) break;
        
        dict.meaning = findWord("meaning=\"", "\"", fileContent.substr(pos));
        if (dict.meaning.empty()) break;
        
        std::string emphasisStr = findWord("emphasis=\"", "\"", fileContent.substr(pos));
        if (!emphasisStr.empty()) {
            dict.emphasis = std::stoul(emphasisStr);
        } else {
            dict.emphasis = 0;
        }
        
        dictionary.push_back(dict);
        
        size_t nextRecord = fileContent.find("word=\"", pos + 1);
        if (nextRecord == std::string::npos) break;
        pos = nextRecord;
    }
}


std::vector<Dictionary> readXmlFromFile() {
    std::vector<Dictionary> dictionary;
    
    std::string xmlContent = fileReader();

    size_t pos = 0;
    const std::string recordStart = "<record>";
    const std::string recordEnd = "</record>";
    
    while ((pos = xmlContent.find(recordStart, pos)) != std::string::npos) {
        size_t endPos = xmlContent.find(recordEnd, pos);
        if (endPos == std::string::npos) break;
        
        std::string recordXml = xmlContent.substr(pos + recordStart.length(), endPos - pos - recordStart.length());
        
        Dictionary dict;
        dict.word = findWord("<word>", "</word>", recordXml);
        dict.meaning = findWord("<meaning>", "</meaning>", recordXml);
        
        std::string emphasisStr = findWord("<emphasis>", "</emphasis>", recordXml);
        if (!emphasisStr.empty()) {
            dict.emphasis = std::stoul(emphasisStr);
        } else {
            dict.emphasis = 0;
        }
        
        dictionary.push_back(dict);
        pos = endPos + recordEnd.length();
    }
    
    return dictionary;
}