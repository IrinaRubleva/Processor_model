#include <iostream>
#include "dictionary.h"
#include <string>
#include <limits>
#include <cstring>
#include "dialog.h"
#include <vector>
#include <algorithm>



int main(){
    std::vector <Dictionary> dictionary;
    while(1){
        unsigned int option;
        try{
            option = showMenuAndGetChoice();
            
            switch (option){
                case 1:{
                    dictionary = readDictionaryFromFile();
                    sortDictionary(dictionary);
                    writeFile_ToXml(dictionary);
                    break;
                }
                case 2:{
                    dictionary = readXmlFromFile();
                    sortDictionary(dictionary);
                    writeFile_FromXml(dictionary);
                    break;
                }
                case 3:{
                    return 0;
                    break;
                }
                default:{
                    throw std::out_of_range("Invalid option. Please choose 1-3.");
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