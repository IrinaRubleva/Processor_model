#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "processor.hpp"
#include <fstream>
#include <memory>


/*!
*@brief Загрузчик. Загружает конфиг, инициализируя сам процессор
*/

class CommandFactory;

class Loader
{
public:
    void loadConfig(const std::string &file, Processor &cpu);
};