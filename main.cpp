#include <iostream>
#include <locale>
#include <filesystem>
#include "ini_parser.h"

namespace fs = std::filesystem;

// ������� ��� ������ config.ini � �������� ������
std::string find_config_file(const std::string& filename, int max_depth = 3)
{
    // ��������� ������� ����������
    if (fs::exists(filename)) {
        return filename;
    }

    // ��������� ������������ ����������
    std::string path = filename;
    for (int i = 0; i < max_depth; ++i) {
        path = "../" + path;
        if (fs::exists(path)) {
            return path;
        }
    }

    // ���� ���� �� ������
    return "";
}

int main()
{
    try
    {
        // ��������� ������ ��� ��������� �������� �����
        setlocale(LC_ALL, "Russian");
        std::locale::global(std::locale(""));

        // ����� config.ini
        std::string config_path = "config.ini";
        std::string found_path = find_config_file(config_path);

        if (found_path.empty())
        {
            // ���� ���� �� ������, ������� ����� � ���������
            std::ofstream out(config_path);
            if (out)
            {
                out << R"(
[Section1]
; ������ ������ � �������� �������������
var1=5.0
var2=������, ���!

[Section2]
var1=42
var2=�������� ������
)";
                out.close();
                std::cout << "������ ����� ���� config.ini � ���������\n";
                found_path = config_path;
            }
            else
            {
                throw ini_parser_error("�� ������� ������� config.ini");
            }
        }

        // ������� ���������� � ��������� �����
        std::cout << "������������ ������ ����: " << fs::absolute(found_path) << "\n\n";

        // ������� ������
        ini_parser parser(found_path);

        // ������� ������ ��������
        try
        {
            int int_val = parser.get_value<int>("Section1.var1");
            double dbl_val = parser.get_value<double>("Section1.var1");
            std::string str_val = parser.get_value<std::string>("Section1.var2");

            std::cout << "����� �����: " << int_val << std::endl;
            std::cout << "������� �����: " << dbl_val << std::endl;
            std::cout << "������: " << str_val << std::endl;
        }
        catch (const ini_parser_error& e)
        {
            std::cerr << "������ ������ ��������: " << e.what() << std::endl;
        }

    }
    catch (const ini_parser_error& e)
    {
        std::cerr << "������ INI-�������: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "����������� ������: " << e.what() << std::endl;
        return 2;
    }
    catch (...)
    {
        std::cerr << "����������� ������!" << std::endl;
        return 3;
    }

    return 0;
}