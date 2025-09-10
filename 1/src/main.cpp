#include <iostream>
#include "dictionary.h"
#include <string>
#include <limits>
#include <cstring>
#include "dialog.h"

int main(){
    Dictionary dictionary;
    while(1){
        unsigned int option;
        try{
            option = showMenuAndGetChoice();
            
            switch (option){
                case 1: {
                    dictionary = inputDictionaryRecord();
                    break;
                }
                case 2:{
                    convertToXml(dictionary);
                    break;
                }
                case 3:{
                    std::string xmlString = getXmlStringFromUser();
                    convertFromXml(xmlString);
                }
                case 4:{
                    return 0;
                    break;
                }
                default:{
                    throw std::out_of_range("Invalid option. Please choose 1-4.");
                    break;
                }
            }
        } catch(const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } 
    return 0;
}