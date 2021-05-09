#ifndef TAR_H
#define TAR_H

#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>


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

class Tar
{
public:

	struct File
	{
		std::string name;
		int mode;
		int ownerId;
		int groupId;
		int size;
		int lastModified;
		tar_type_flag typeflag;
		std::string linkedName;
		size_t position;
	};

	Tar(std::string filename);
	~Tar();
	void extract(const File& file, std::string to);
	bool extract(std::string fileName, std::string to);
	const std::vector<File>& getFileList() const;
private:
	FILE* tarfile = NULL;
	std::vector<File> fileList;

	int oct_to_int(char* oct);
	int oct_to_sz(char* oct);

	static constexpr int EXTRACT_BUFFER_SZ = 1024 * 16;
};

#endif // TAR_H