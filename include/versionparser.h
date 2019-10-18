#ifndef VERSIONPARSER_H
#define VERSIONPARSER_H

#include <string>
#include <regex>
#include <list>
#include <sstream>

#include "config.h"

// Auxiliary class for version comparation
class Version {
public:
	// Version parse exception
	class VersionException : public std::exception
	{
	public:
		explicit VersionException(const char* message) :
			msg_(message)
		{
		}
		explicit VersionException(const std::string& message) :
			msg_(message)
		{}
		virtual ~VersionException() throw () {}
		virtual const char* what() const throw () {
			return msg_.c_str();
		}

	protected:
		std::string msg_;
	};

	enum VersionNumberType {
		MAJOR,
		MINOR,
		REVISION
	};

	Version(std::string version)
		: version(version)
	{
		std::smatch mres;
		if (std::regex_match(version, mres, pattern)) {
			major = std::stoi(mres.str(1));
			minor = std::stoi(mres.str(2));
			revision = std::stoi(mres.str(3));
		}
		else throw VersionException("Not a valid version string");
	}

	std::string getVersionText() const {
		return version;
	}

	int getVersionNumber(VersionNumberType vnType = MAJOR) const {
		switch (vnType) {
		default:
		case MAJOR:
			return major;
		case MINOR:
			return minor;
		case REVISION:
			return revision;
		}
	}

	bool operator== (const Version& v2) const {
		return (major == v2.major &&
			minor == v2.minor &&
			revision == v2.revision
			);
	}

	bool operator!= (const Version& v2) const {
		return !(*this == v2);
	}

	bool operator> (const Version& v2) const {
		if (major > v2.major)
			return true;
		if (major == v2.major)
		{
			if (minor > v2.minor)
				return true;
			if (minor == v2.minor)
			{
				if (revision > v2.revision)
					return true;
			}
		}

		return false;
	}

	bool operator< (const Version& v2) const {
		return !(*this > v2) && *this != v2;
	}

	bool operator>= (const Version& v2) const {
		return (*this > v2) || (*this == v2);
	}

	bool operator<= (const Version& v2) const {
		return (*this < v2) || (*this == v2);
	}
private:
	std::string version;
	std::regex pattern = std::regex("(\\d+)\\.(\\d+)\\.(\\d+)");
	int major = 0, minor = 0, revision = 0;
};

class VersionParser
{
public:
	VersionParser(const std::string& versionhtmltext);
	const Version& getLatest() const;
	std::list<Version>::const_iterator cbegin() const;
	std::list<Version>::const_iterator cend() const;
	int size() const;
private:
	std::string htmltext;
	std::list<Version> versions;
	std::regex pattern = std::regex(VERSION_REGEX_PATTERN, std::regex::icase);

};

#endif // VERSIONPARSER_H
