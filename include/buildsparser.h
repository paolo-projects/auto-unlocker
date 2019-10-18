#ifndef BUILDSPARSER_H
#define BUILDSPARSER_H

#include <string>
#include <regex>
#include <list>
#include <sstream>

#include "config.h"

class BuildsParser
{
public:
	BuildsParser(const std::string& buildshtmltext);
	const std::string& getLatest() const;
	std::list<std::string>::const_iterator cbegin() const;
	std::list<std::string>::const_iterator cend() const;
	int size() const;
private:
	std::string htmltext;
	std::list<std::string> builds;
	std::regex pattern = std::regex(VERSION_REGEX_PATTERN, std::regex::icase);
};

#endif // BUILDSPARSER_H
