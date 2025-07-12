#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <sstream>
#include <stdexcept>
#include <cctype>

class ini_parser_error : public std::runtime_error
{
public:
    explicit ini_parser_error(const std::string& msg, int line = -1)
        : std::runtime_error(line == -1 ? msg : "Ошибка в строке " + std::to_string(line) + ": " + msg)
    {
    }
};

class ini_parser
{
private:
    std::map<std::string, std::map<std::string, std::string>> data;
    std::string filename;

    std::string trim(const std::string& str);
    std::pair<std::string, std::string> split_key_value(const std::string& line, int line_num);
    void validate_section_name(const std::string& name, int line_num);
    void validate_key_name(const std::string& name, int line_num);
    void parse_file();

public:
    explicit ini_parser(const std::string& filename);

    template<typename T>
    T get_value(const std::string& key_path)
    {
        std::string str_value = get_value_as_string(key_path);
        std::istringstream iss(str_value);
        T result;

        if (!(iss >> result))
        {
            throw ini_parser_error("Не удалось преобразовать значение '" + str_value + "' к запрошенному типу");
        }

        return result;
    }

    std::string get_value_as_string(const std::string& key_path);
};