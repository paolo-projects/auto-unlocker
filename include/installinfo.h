#ifndef INSTALLINFOUTILS_H
#define INSTALLINFOUTILS_H

#include <string>
#include <stdexcept>

#include "config.h"

class VMWareInfoException : public std::runtime_error
{
public:
	VMWareInfoException(const char* message) : std::runtime_error(message) {}
	VMWareInfoException(const std::string& message) : std::runtime_error(message) {}
};

class VMWareInfoRetriever
{
public:
	VMWareInfoRetriever();
	std::string getInstallPath();
	std::string getInstallPath64();
	std::string getProductVersion();

private:
	std::string installPath, installPath64, prodVersion;
};

#endif // INSTALLINFOUTILS_H
