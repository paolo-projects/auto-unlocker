#ifndef UNLOCKER_ZIP_H
#define UNLOCKER_ZIP_H

#include <string>
#include <stdexcept>
#include <array>

#define LIBZIP_STATIC

#include <zipconf.h>
#include <zip.h>

class ZipException : public std::runtime_error
{
public:
	ZipException(const char* message) : std::runtime_error(message) {}
	ZipException(const std::string& message) : std::runtime_error(message) {}
};

class Zip {
public:
	Zip(const std::string& zipFile);
	~Zip();
	bool extract(const std::string& fileName, const std::string& to, void(*progressCallback)(float) = nullptr);
private:
	static constexpr size_t AR_BUFFER_SIZE = 1024*16;
	zip_t* zip_archive = nullptr;
};


#endif // UNLOCKER_ZIP_H