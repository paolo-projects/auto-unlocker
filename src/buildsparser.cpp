#include "buildsparser.h"

BuildsParser::BuildsParser(const std::string& buildshtmltext)
	:	htmltext(buildshtmltext)
{
	std::istringstream iss(htmltext);

	std::string line;
	while (std::getline(iss, line))
	{
		std::smatch vmatch;
		if (std::regex_match(line, vmatch, pattern))
		{
			builds.emplace_back(vmatch.str(1));
		}
	}
}

const std::string& BuildsParser::getLatest() const
{
	if (builds.size() == 0)
		throw std::runtime_error("No elements in the list");

	return *builds.cbegin();
}

std::list<std::string>::const_iterator BuildsParser::cbegin() const
{
	return builds.cbegin();
}

std::list<std::string>::const_iterator BuildsParser::cend() const
{
	return builds.end();
}

int BuildsParser::size() const
{
	return builds.size();
}
