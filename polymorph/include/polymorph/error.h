#pragma once

#include <string>
#include <stdexcept>

inline void print_error(const std::string& error_type, const std::string& expression, const std::string& file, int line)
{
    printf("%s error @ %s (file: %s, line: %d)\n", error_type.c_str(), expression.c_str(), file.c_str(), line);
    throw std::runtime_error(expression);
}

inline void print_warn(const std::string& warn_type, const std::string& text)
{
    printf("%s error: %s\n", warn_type.c_str(), text.c_str());
}