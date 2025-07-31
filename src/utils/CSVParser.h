#pragma once
#include <string>
#include <vector>

class CSVParser {
public:
    CSVParser(const std::string &line);
    const std::vector<std::string>& fields() const;
private:
    std::vector<std::string> _fields;
};