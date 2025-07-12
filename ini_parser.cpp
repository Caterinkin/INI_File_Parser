#include "ini_parser.h"

std::string ini_parser::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos)
    {
        return "";
    }
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

std::pair<std::string, std::string> ini_parser::split_key_value(const std::string& line, int line_num)
{
    size_t eq_pos = line.find('=');
    if (eq_pos == std::string::npos)
    {
        throw ini_parser_error("Некорректный формат строки (отсутствует '=')", line_num);
    }

    std::string key = trim(line.substr(0, eq_pos));
    if (key.empty())
    {
        throw ini_parser_error("Пустой ключ", line_num);
    }

    std::string value = trim(line.substr(eq_pos + 1));
    return std::make_pair(key, value);
}

void ini_parser::validate_section_name(const std::string& name, int line_num)
{
    if (name.empty())
    {
        throw ini_parser_error("Пустое имя секции", line_num);
    }

    for (char c : name)
    {
        if (std::isspace(c))
        {
            throw ini_parser_error("Имя секции содержит пробелы", line_num);
        }
    }
}

void ini_parser::validate_key_name(const std::string& name, int line_num)
{
    if (name.empty())
    {
        throw ini_parser_error("Пустой ключ", line_num);
    }

    for (char c : name)
    {
        if (std::isspace(c))
        {
            throw ini_parser_error("Ключ содержит пробелы", line_num);
        }
    }
}

void ini_parser::parse_file()
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw ini_parser_error("Не удалось открыть файл: " + filename);
    }

    std::locale::global(std::locale(""));
    file.imbue(std::locale());

    std::string current_section;
    std::string line;
    int line_num = 0;

    while (std::getline(file, line))
    {
        line_num++;
        line = trim(line);

        if (line.empty() || line[0] == ';')
        {
            continue;
        }

        if (line[0] == '[')
        {
            if (line.back() != ']')
            {
                throw ini_parser_error("Некорректное объявление секции - отсутствует ']'", line_num);
            }

            current_section = trim(line.substr(1, line.size() - 2));
            validate_section_name(current_section, line_num);
            data[current_section];
            continue;
        }

        if (current_section.empty())
        {
            throw ini_parser_error("Ключ-значение вне секции", line_num);
        }

        auto kv_pair = split_key_value(line, line_num);
        validate_key_name(kv_pair.first, line_num);
        data[current_section][kv_pair.first] = kv_pair.second;
    }
}

ini_parser::ini_parser(const std::string& filename) : filename(filename)
{
    parse_file();
}

std::string ini_parser::get_value_as_string(const std::string& key_path)
{
    size_t dot_pos = key_path.find('.');
    if (dot_pos == std::string::npos)
    {
        throw ini_parser_error("Некорректный формат ключа (отсутствует '.')");
    }

    std::string section = key_path.substr(0, dot_pos);
    std::string key = key_path.substr(dot_pos + 1);

    if (section.empty() || key.empty())
    {
        throw ini_parser_error("Пустое имя секции или ключа");
    }

    auto section_it = data.find(section);
    if (section_it == data.end())
    {
        std::vector<std::string> available_sections;
        for (const auto& [sec, _] : data)
        {
            available_sections.push_back(sec);
        }

        std::string hint = "Доступные секции: ";
        for (size_t i = 0; i < available_sections.size(); ++i)
        {
            if (i != 0) hint += ", ";
            hint += available_sections[i];
        }

        throw ini_parser_error("Секция '" + section + "' не найдена. " + hint);
    }

    auto key_it = section_it->second.find(key);
    if (key_it == section_it->second.end())
    {
        std::vector<std::string> available_keys;
        for (const auto& [k, _] : section_it->second)
        {
            available_keys.push_back(k);
        }

        std::string hint = "Доступные ключи в секции '" + section + "': ";
        for (size_t i = 0; i < available_keys.size(); ++i)
        {
            if (i != 0) hint += ", ";
            hint += available_keys[i];
        }

        throw ini_parser_error("Ключ '" + key + "' не найден в секции '" + section + "'. " + hint);
    }

    return key_it->second;
}