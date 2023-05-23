#ifndef TOOLSDOWNLOADER_H
#define TOOLSDOWNLOADER_H

#include <string>
#include "filesystem.hpp"
#include "network.h"
#include "archive.h"
#include "buildsparser.h"
#include "debug.h"

class ToolsDownloaderException : public std::runtime_error
{
public:
	ToolsDownloaderException(const char* message) : std::runtime_error(message) {}
	ToolsDownloaderException(const std::string& message) : std::runtime_error(message) {}
};

class ToolsDownloader
{
public:
	ToolsDownloader(Network& network, const std::string& baseUrl, const std::string& version);
	bool download(const fs::path& to);
private:
	Network& network;
	std::string baseUrl, versionNumber, versionUrl, buildurl;

	bool downloadFromCore(const fs::path& to);
};

#endif // TOOLSDOWNLOADER_H