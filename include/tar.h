#ifndef TAR_H
#define TAR_H

#include <string>
#include <vector>
#include <array>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <functional>

#ifdef WIN32
#define fseek_e64(f, p, t) _fseeki64(f, p, t);
#else
#define fseek_e64(f, p, t) fseeko(f, p, t);
#endif

constexpr int TAR_BLOCK_SZ = 512;

enum tar_type_flag {
	REGTYPE = '0',		/* regular file */
	AREGTYPE = '\0',	/* regular file */
	LNKTYPE = '1',		/* link */
	SYMTYPE = '2',		/* reserved */
	CHRTYPE = '3',		/* character special */
	BLKTYPE = '4',		/* block special */
	DIRTYPE = '5',		/* directory */
	FIFOTYPE = '6',		/* FIFO special */
	CONTTYPE = '7',		/* reserved */
	XHDTYPE = 'x',		/* Extended header referring to the next file in the archive */
	XGLTYPE = 'g'		/* Global extended header */
};

struct tar_posix_header
{                              /* byte offset */
	char name[100];               /*   0 */
	char mode[8];                 /* 100 */
	char uid[8];                  /* 108 */
	char gid[8];                  /* 116 */
	char size[12];                /* 124 */
	char mtime[12];               /* 136 */
	char chksum[8];               /* 148 */
	char typeflag;                /* 156 */
	char linkname[100];           /* 157 */
	char magic[6];                /* 257 */
	char version[2];              /* 263 */
	char uname[32];               /* 265 */
	char gname[32];               /* 297 */
	char devmajor[8];             /* 329 */
	char devminor[8];             /* 337 */
	char prefix[155];             /* 345 */
	char pad[12];				  /* 500 */
								  /* 512 */
};

class TarException : public std::runtime_error {
public:
	TarException(const char* message) : std::runtime_error(message) {}
	TarException(const std::string& message) : std::runtime_error(message) {}
};

class Tar
{
public:

	struct File
	{
		std::string name;
		int mode;
		int ownerId;
		int groupId;
		size_t size;
		int lastModified;
		tar_type_flag typeflag;
		std::string linkedName;
		size_t position;
	};

	Tar(std::string filename);
	~Tar();
	void extract(const File& file, std::string to, void(*progressCallback)(float) = nullptr);
	bool extract(std::string fileName, std::string to, void(*progressCallback)(float) = nullptr);
	const std::vector<File>& getFileList() const;
	bool contains(std::string fileName) const;
	std::vector<File> search(const std::string& term);
private:
	void(*progressCallback)(float) = nullptr;
	FILE* tarfile = NULL;
	std::vector<File> fileList;

	int octToInt(char* oct);
	size_t octToSz(char* oct);

	static constexpr size_t EXTRACT_BUFFER_SZ = 1024 * 16;
};

#endif // TAR_H