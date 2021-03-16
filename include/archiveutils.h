#ifndef ARCHIVEUTILS_H
#define ARCHIVEUTILS_H

#include "filesystem.hpp"
/*
#include <archive.h>
#include <archive_entry.h>
*/
#ifdef __linux__
#include <cstring>
#endif

#define LIBZIP_STATIC

#include "zipconf.h"
#include "zip.h"
#include "tar.h"
#include "debugutils.h"

#define AR_BUFFER_SIZE 4096

namespace Archive
{
	/*int copy_data(struct archive* ar, struct archive* aw);
	bool extract(const char* from, const char* filename, const char* to);
	bool extract_s(fs::path from, std::string filename, fs::path to);*/
	bool extract_zip(fs::path from, std::string filename, fs::path to);
	bool extract_tar(fs::path from, std::string filename, fs::path to);
}

#endif // ARCHIVEUTILS_H
