#ifndef PATCHRESULT_H
#define PATCHRESULT_H

#include <string>

struct PatchResult
{
	bool result;
	std::string errorMessage;
	std::string logFilePath;
};

#endif