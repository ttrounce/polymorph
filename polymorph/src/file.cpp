#include "polymorph/io/file.h"
#include "polymorph/error.h"

#include <fstream>

#define ASSERT(expr) { if(!(expr)) { print_error("File IO", #expr, __FILE__, __LINE__); } }

using namespace poly;

std::vector<char> poly::read_file_vec_u8(const std::string& path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		print_warn("File IO", "file '" + path + "' not found ");
		throw std::runtime_error("file '" + path + "' not found "); // TODO: could be improved.
	}

	size_t file_size = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(file_size);

	file.seekg(0);
	file.read(buffer.data(), file_size);

	file.close();

	return buffer;
}

std::string poly::read_file_str(const std::string& path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		print_warn("File IO", "file '" + path + "' not found ");
		throw std::runtime_error("file '" + path + "' not found ");
	}

	size_t file_size = static_cast<size_t>(file.tellg());
	char* buffer = new char[file_size + 1];

	file.seekg(0);
	file.read(buffer, file_size);

	buffer[file_size] = '\0';

	file.close();

	std::string str(buffer);

	delete[] buffer;

	return str;
}