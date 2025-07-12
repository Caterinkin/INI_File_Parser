#include <iostream>
#include <locale>
#include <filesystem>
#include "ini_parser.h"

namespace fs = std::filesystem;

// Функция для поиска config.ini в соседних папках
std::string find_config_file(const std::string& filename, int max_depth = 3)
{
    // Проверяем текущую директорию
    if (fs::exists(filename)) {
        return filename;
    }

    // Проверяем родительские директории
    std::string path = filename;
    for (int i = 0; i < max_depth; ++i) {
        path = "../" + path;
        if (fs::exists(path)) {
            return path;
        }
    }

    // Если файл не найден
    return "";
}

int main()
{
    try
    {
        // Установка локали для поддержки русского языка
        setlocale(LC_ALL, "Russian");
        std::locale::global(std::locale(""));

        // Поиск config.ini
        std::string config_path = "config.ini";
        std::string found_path = find_config_file(config_path);

        if (found_path.empty())
        {
            // Если файл не найден, создаем новый с примерами
            std::ofstream out(config_path);
            if (out)
            {
                out << R"(
[Section1]
; Пример секции с русскими комментариями
var1=5.0
var2=Привет, мир!

[Section2]
var1=42
var2=Тестовая строка
)";
                out.close();
                std::cout << "Создан новый файл config.ini с примерами\n";
                found_path = config_path;
            }
            else
            {
                throw ini_parser_error("Не удалось создать config.ini");
            }
        }

        // Выводим информацию о найденном файле
        std::cout << "Используется конфиг файл: " << fs::absolute(found_path) << "\n\n";

        // Создаем парсер
        ini_parser parser(found_path);

        // Примеры чтения значений
        try
        {
            int int_val = parser.get_value<int>("Section1.var1");
            double dbl_val = parser.get_value<double>("Section1.var1");
            std::string str_val = parser.get_value<std::string>("Section1.var2");

            std::cout << "Целое число: " << int_val << std::endl;
            std::cout << "Дробное число: " << dbl_val << std::endl;
            std::cout << "Строка: " << str_val << std::endl;
        }
        catch (const ini_parser_error& e)
        {
            std::cerr << "Ошибка чтения значений: " << e.what() << std::endl;
        }

    }
    catch (const ini_parser_error& e)
    {
        std::cerr << "Ошибка INI-парсера: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Неожиданная ошибка: " << e.what() << std::endl;
        return 2;
    }
    catch (...)
    {
        std::cerr << "Неизвестная ошибка!" << std::endl;
        return 3;
    }

    return 0;
}