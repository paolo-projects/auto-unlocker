#include "versionparser.h"

VersionParser::VersionParser(const std::string& versionhtmltext)
	: htmltext(versionhtmltext)
{
	std::istringstream iss(htmltext);

	std::string line;
	while (std::getline(iss, line))
	{
		try
		{
			std::smatch vmatch;
			if (std::regex_match(line, vmatch, pattern))
			{
				Version v(vmatch.str(1));
				versions.emplace_back(v);
			}
		}
		catch (const Version::VersionException & exc)
		{
			// Not a good version number
			// Could implement debug messages
		}
	}

	// Sort by latest first
	versions.sort(std::greater<>());
}

const Version& VersionParser::getLatest() const
{
	if (versions.size() == 0)
		throw std::runtime_error("No elements in the list");

	return *versions.cbegin();
}

std::list<Version>::const_iterator VersionParser::cbegin() const
{
	return versions.cbegin();
}

std::list<Version>::const_iterator VersionParser::cend() const
{
	return versions.cend();
}

int VersionParser::size() const
{
	return versions.size();
}
