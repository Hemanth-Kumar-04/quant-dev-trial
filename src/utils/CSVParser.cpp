#include "CSVParser.h"
#include <sstream>

CSVParser::CSVParser(const std::string &line) {
    std::stringstream ss(line);
    std::string cell;
    while (std::getline(ss, cell, ',')) {
        _fields.push_back(cell);
    }
}

const std::vector<std::string>& CSVParser::fields() const {
    return _fields;
}