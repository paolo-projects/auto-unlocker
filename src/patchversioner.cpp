#include "patchversioner.h"

PatchVersioner::PatchVersioner(const fs::path& installPath)
{
	versionFile = (installPath / PATCH_VER_FILE).string();

	FILE* vFile = fopen(versionFile.c_str(), "rb");

	if (vFile != NULL)
	{
		vData = {};

		fread(&vData, sizeof(patch_version_t), 1, vFile);

		fclose(vFile);

		haspatch = true;
	}
}

time_t PatchVersioner::getPatchTime() const
{
	time_t pTime;
	sscanf(vData.timestamp, "%lld", &pTime);
	return pTime;
}

std::string PatchVersioner::getPatchVersion() const
{
	return std::string(vData.version);
}

void PatchVersioner::writePatchData()
{
	FILE* vFile = fopen(versionFile.c_str(), "wb");

	if (vFile == NULL)
	{
		throw PatchVersionerException("Can't open version file for writing at: " + versionFile);
	}

	time_t pTime = time(NULL);

	vData = {};
	sprintf(vData.timestamp, "%lld", pTime);
	strcpy(vData.version, PROG_VERSION);

	fwrite(&vData, sizeof(patch_version_t), 1, vFile);
	fclose(vFile);

#ifdef _WIN32
	// Set file to hidden on Windows
	SetFileAttributes(versionFile.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY);
#endif
}

void PatchVersioner::removePatchVersion()
{
	fs::remove(versionFile);
}

bool PatchVersioner::hasPatch() const
{
	return haspatch;
}
