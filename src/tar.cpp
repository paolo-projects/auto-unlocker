#include "tar.h"

Tar::Tar(std::string filename)
{
	tarfile = fopen(filename.c_str(), "rb");

	if (tarfile == NULL)
	{
		throw std::runtime_error("Couldn't open " + filename);
	}

	tar_posix_header fileheader = {};

	int readsz = 0;
	while ((readsz = fread(reinterpret_cast<char*>(&fileheader), 1, sizeof(fileheader), tarfile)) == 512)
	{
		int checksum = 0;
		char* header_data = reinterpret_cast<char*>(&fileheader);

		for (int i = 0; i < sizeof(tar_posix_header); i++)
			checksum += ((unsigned char*)(&(fileheader)))[i];
		for (int i = 0; i < 8; i++)
			checksum += (' ' - (unsigned char)fileheader.chksum[i]);

		int header_checksum = oct_to_int(fileheader.chksum);

		if (checksum != header_checksum) {
			continue;
		}

		File file;
		file.name = std::string(fileheader.name);

		switch (fileheader.size[0]) {
		case 0xFF: // base-256
			throw std::runtime_error("base-256 size encoding unsupported");
			break;
		case 0x80:
			throw std::runtime_error("base-256 size encoding unsupported");
			break;
		default: // octal
			file.size = oct_to_sz(fileheader.size);
			break;
		}
		file.groupId = oct_to_int(fileheader.gid);
		file.mode = oct_to_int(fileheader.mode);
		file.ownerId = oct_to_int(fileheader.uid);
		file.lastModified = oct_to_int(fileheader.mtime);
		file.position = ftell(tarfile);
		file.typeflag = static_cast<tar_type_flag>(fileheader.typeflag);

		fileList.emplace_back(file);

		int toskip = (std::ceil(file.size / 512.0) * 512);
		fseek(tarfile, toskip, SEEK_CUR);
	}

	fseek(tarfile, 0, SEEK_SET);
}

Tar::~Tar()
{
	if (tarfile != NULL) {
		fclose(tarfile);
		tarfile = NULL;
	}
}

void Tar::extract(const File& file, std::string to)
{
	if (file.typeflag != REGTYPE && file.typeflag != AREGTYPE) {
		throw std::runtime_error("Can't extract this type of file. Only regular files supported");
	}

	fseek(tarfile, file.position, SEEK_SET);
	int size = file.size;

	FILE* outfile = fopen(to.c_str(), "wb");

	if (outfile != NULL) {
		char* buffer = new char[EXTRACT_BUFFER_SZ];

		while (size > 0) {
			int tocopy = std::min(EXTRACT_BUFFER_SZ, size);
			fread(buffer, 1, tocopy, tarfile);
			fwrite(buffer, 1, tocopy, outfile);

			size -= tocopy;
		}
		delete[] buffer;
		fclose(outfile);
	}
	else {
		throw std::runtime_error("Error while opening " + to + " for extracting");
	}
}


bool Tar::extract(std::string fileName, std::string to)
{
	for (const File& file : fileList) {
		if (file.name == fileName) {
			extract(file, to);
			return true;
		}
	}
	return false;
}

const std::vector<Tar::File>& Tar::getFileList() const
{
	return fileList;
}


int Tar::oct_to_int(char* oct)
{
	int res;
	return sscanf(oct, "%o", &res) == 1 ? res : 0;
}

int Tar::oct_to_sz(char* oct)
{
	size_t res;
	return sscanf(oct, "%zo", &res) == 1 ? res : 0;
}
