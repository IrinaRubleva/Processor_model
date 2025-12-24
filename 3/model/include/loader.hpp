#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "processor.hpp"
#include <fstream>
#include <memory>

class Loader
{
public:
    void loadConfig(const std::string &file, Processor &cpu);
};
