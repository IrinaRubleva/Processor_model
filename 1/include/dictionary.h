#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <vector>
#include <string>

struct Dictionary{
    std::string word;
    std::string meaning;
    size_t emphasis;
};


char* toXml(const Dictionary& record, const char* recordName);
char* toXml(const Dictionary& record, const char* recordName, size_t nameSize);
std::string toXml(const Dictionary& record, const std::string& recordName);

Dictionary fromXml(const char* xmlString, const char* recordName);
Dictionary fromXml(const char* xmlString, size_t xmlSize, const char* recordName, size_t nameSize);
Dictionary fromXml(const std::string& xmlString, const std::string& recordName);

void writeFile_ToXml(const std::vector<Dictionary>& dictionary);
void writeFile_FromXml(const std::vector<Dictionary>& dictionary);
std::vector <Dictionary> readDictionaryFromFile();
std::vector <Dictionary> readXmlFromFile();

void sortDictionary(std::vector <Dictionary> &dict);

#endif