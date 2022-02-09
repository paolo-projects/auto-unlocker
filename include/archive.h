#ifndef ARCHIVEUTILS_H
#define ARCHIVEUTILS_H

#include "filesystem.hpp"
#ifdef __linux__
#include <cstring>
#endif

#include "ziparchive.h"
#include "tar.h"
#include "debug.h"
#include <errno.h>

class Archive
{
public:
	static bool extractZip(fs::path from, std::string filename, fs::path to);
	static bool extractTar(fs::path from, std::string filename, fs::path to);
	static void extractionProgress(float progress);
};

#endif // ARCHIVEUTILS_H
