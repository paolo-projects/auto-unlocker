#ifndef INSTALLINFOUTILS_H
#define INSTALLINFOUTILS_H

#include <string>

#include "config.h"

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
