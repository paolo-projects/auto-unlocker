#ifndef PATCHVERSIONER_H
#define PATCHVERSIONER_H

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdexcept>
#include <cstdio>
#include <string>
#include <cstring>

#include "filesystem.hpp"
#include "config.h"

class PatchVersionerException : public std::runtime_error
{
public:
	PatchVersionerException(const char* msg) : std::runtime_error(msg) {}
	PatchVersionerException(const std::string& msg) : std::runtime_error(msg) {}
};

class PatchVersioner
{
public:
	PatchVersioner(const fs::path& installPath);
	time_t getPatchTime() const;
	std::string getPatchVersion() const;
	void writePatchData();
	void removePatchVersion();
	bool hasPatch() const;

private:
	typedef struct _patch_version_t {
		char timestamp[256];
		char version[256];
	} patch_version_t;

	std::string versionFile;
	bool haspatch = false;
	patch_version_t vData = {};
};

#endif