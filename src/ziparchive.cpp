#include "ziparchive.h"

Zip::Zip(const std::string& zipFile)
{
	// Open the zip file
	const char* from_c = zipFile.c_str();
	int zerr = ZIP_ER_OK;
	zip_archive = zip_open(from_c, ZIP_RDONLY, &zerr);

	if (zerr != ZIP_ER_OK) {
		throw ZipException("Error while opening " + zipFile);
	}
}

Zip::~Zip()
{
	if (zip_archive != nullptr) {
		zip_close(zip_archive);
		zip_archive = nullptr;
	}
}

bool Zip::extract(const std::string& fileName, const std::string& to, void(*progressCallback)(float))
{
	const char* fileName_c = fileName.c_str(), * to_c = to.c_str();

	// Get the size of the file to extract
	zip_stat_t target_file_stat;
	zip_stat_init(&target_file_stat);
	int res = zip_stat(zip_archive, fileName_c, 0, &target_file_stat);

	if (res != 0)
	{
		return false;
	}

	size_t target_fsize = target_file_stat.size;

	// Open the file to extract
	zip_file_t* target_file = zip_fopen(zip_archive, fileName_c, 0);

	if (target_file == NULL) {
		return false;
	}

	// Open the output file
	FILE* out_f = fopen(to_c, "wb");

	if (out_f == NULL) {
		throw ZipException("Can't open the file " + to + " for writing");
	}

	size_t elapsed = 0;

	// Extract the file, printing the progress through the function
	std::array<char, AR_BUFFER_SIZE> buffer;
	size_t sz = 0;
	while ((sz = zip_fread(target_file, buffer.data(), AR_BUFFER_SIZE)) > 0) {
		fwrite(buffer.data(), sizeof(char), sz, out_f);
		elapsed += sz;
		if (progressCallback != nullptr) {
			progressCallback(static_cast<float>(elapsed) / target_fsize);
		}
	}

	fclose(out_f);

	zip_fclose(target_file);

	return true;
}
