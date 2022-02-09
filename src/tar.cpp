#include "tar.h"

Tar::Tar(std::string filename)
{
	tarfile = fopen(filename.c_str(), "rb");

	if (tarfile == NULL)
	{
		throw TarException("Couldn't open " + filename);
	}

	tar_posix_header fileheader = {};

	int readsz = 0;
	while ((readsz = fread(reinterpret_cast<char*>(&fileheader), 1, sizeof(fileheader), tarfile)) == TAR_BLOCK_SZ)
	{
		int checksum = 0;
		char* header_data = reinterpret_cast<char*>(&fileheader);

		for (int i = 0; i < sizeof(tar_posix_header); i++)
			checksum += ((unsigned char*)(&(fileheader)))[i];
		for (int i = 0; i < 8; i++)
			checksum += (' ' - (unsigned char)fileheader.chksum[i]);

		int header_checksum = octToInt(fileheader.chksum);

		if (checksum != header_checksum) {
			continue;
		}

		File file;
		file.name = std::string(fileheader.name);

		file.size = octToSz(fileheader.size);
		file.groupId = octToInt(fileheader.gid);
		file.mode = octToInt(fileheader.mode);
		file.ownerId = octToInt(fileheader.uid);
		file.lastModified = octToInt(fileheader.mtime);
		file.position = ftell(tarfile);
		file.typeflag = static_cast<tar_type_flag>(fileheader.typeflag);

		fileList.emplace_back(file);

		// Calculate the bytes to skip to reach the next file header location
		// The count of bytes to skip is the file size fitted to the 512-block size
		size_t toskip = (std::ceil(file.size / static_cast<double>(TAR_BLOCK_SZ)) * static_cast<size_t>(TAR_BLOCK_SZ));

		// Using 64-bit data types and functions to support archives containing large files
		// Otherwise, the first large file size (> ~2GB on Windows) would probably overflow the signed long and 
		// break the 512 blocks succession. This is compiler dependent, though
		// 64 bit long long instead supports files way beyond the tar individual file size limit
		fseek_e64(tarfile, toskip, SEEK_CUR);
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

void Tar::extract(const File& file, std::string to, void(*progressCallback)(float))
{
	// If the name ends with a / character then it's a directory and we don't support it (for now)
	if (*(file.name.cend() - 1) == '/') {
		throw TarException("Can't extract a directory. Operation not supported");
	}

	// Throw an exception if the file being extracted is not a regular file (symlink, directory, etc.)
	if (file.typeflag != REGTYPE && file.typeflag != AREGTYPE) {
		throw TarException("Can't extract this type of file. Only regular files supported");
	}

	// Set the file pointer to the beginning of the file being extracted
	fseek_e64(tarfile, file.position, SEEK_SET);

	size_t size, totalSize;
	size = totalSize = file.size;

	FILE* outfile = fopen(to.c_str(), "wb");

	if (outfile != NULL) {
		std::array<char, EXTRACT_BUFFER_SZ> buffer;

		while (size > 0) {
			int tocopy = std::min(EXTRACT_BUFFER_SZ, size);
			fread(buffer.data(), 1, tocopy, tarfile);
			fwrite(buffer.data(), 1, tocopy, outfile);

			if (progressCallback != nullptr) {
				progressCallback(float(totalSize - size) / totalSize);
			}

			size -= tocopy;
		}

		fclose(outfile);
	}
	else {
		throw TarException("Error while opening " + to + " for writing");
	}
}


bool Tar::extract(std::string fileName, std::string to, void(*progressCallback)(float))
{
	for (const File& file : fileList) {
		if (file.name == fileName) {
			extract(file, to, progressCallback);
			return true;
		}
	}
	return false;
}

const std::vector<Tar::File>& Tar::getFileList() const
{
	return fileList;
}

bool Tar::contains(std::string fileName) const
{
	for (const File& f : fileList) {
		if (f.name == fileName) {
			return true;
		}
	}
	return false;
}

std::vector<Tar::File> Tar::search(const std::string& term)
{
	std::vector<File> results;

	std::copy_if(fileList.begin(), fileList.end(), std::back_inserter(results), [&term](const File& file) {
		return file.name.find(term) != std::string::npos;
		});

	return results;
}


int Tar::octToInt(char* oct)
{
	int res;
	return sscanf(oct, "%o", &res) == 1 ? res : 0;
}

size_t Tar::octToSz(char* oct)
{
	if (*oct == 0xFF) {
		throw TarException("base-256 file size not supported.");
	}
	else if (*oct == 0x80) {
		throw TarException("base-256 file size not supported.");
	}
	else {
		size_t res;
		return sscanf(oct, "%zo", &res) == 1 ? res : 0;
	}
}
