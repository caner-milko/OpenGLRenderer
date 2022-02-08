#include <Utils/FileUtils.h>
#include <string>
#include <fstream>
#include <sstream>
std::string FileUtils::readFile(const char *path)
{
	std::string str;
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		file.open(path);
		std::stringstream strStream;
		strStream << file.rdbuf();
		file.close();
		str = strStream.str();
	}
	catch(std::ifstream::failure e)
	{
		std::cout << "ERROR::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	return str;
}