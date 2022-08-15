#pragma once

#include <vector>
#include <string>

namespace poly
{
    std::vector<char> read_file_vec_u8(const std::string& path);
    std::string read_file_str(const std::string& path);
}