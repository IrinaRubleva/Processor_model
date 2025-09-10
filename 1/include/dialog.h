#ifndef DIALOG_H
#define DIALOG_H

#include "dictionary.h"


void convertToXml(const Dictionary& dictionary);
void convertFromXml(const std::string& xmlString);
Dictionary inputDictionaryRecord();
std::string getXmlStringFromUser();
unsigned int showMenuAndGetChoice();

#endif