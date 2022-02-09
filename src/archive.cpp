#include "archive.h"

bool Archive::extractTar(fs::path from, std::string filename, fs::path to)
{
	try
	{
		Tar tarfile(from.string());
		if (!tarfile.extract(filename, to.string(), Archive::extractionProgress)) {
			fprintf(stderr, "TAR: Error while extracting %s. Not in the archive\n", filename.c_str());
			return false;
		}
		printf("\n");
		return true;
	}
	catch (const std::exception& exc)
	{
		fprintf(stderr, "TAR: An error occurred while extracting %s. %s", from.string().c_str(), exc.what());
		return false;
	}
}

bool Archive::extractZip(fs::path from, std::string filename, fs::path to)
{
	try {
		Zip zip(from.string());
		if (!zip.extract(filename, to.string(), Archive::extractionProgress)) {
			fprintf(stderr, "ZIP: Error while extracting %s. Not in the archive\n", filename.c_str());
			return false;
		}
		printf("\n");

		return true;
	}
	catch (const std::exception& exc) {
		fprintf(stderr, "ZIP: An error occurred while extracting %s. %s", from.string().c_str(), exc.what());
		return false;
	}
}

void Archive::extractionProgress(float progress) {
	printf("Extraction progress: %.0f %%  \r", progress*100);
	fflush(stdout);
}