#include "toolsdownloader.h"

ToolsDownloader::ToolsDownloader(Network& network, const std::string& baseUrl, const std::string& version)
	: network(network), versionUrl(baseUrl + version + "/"), versionNumber(version)
{
	std::string versionHtml = network.curlGet(versionUrl);
	BuildsParser builds(versionHtml);

	if (builds.size() == 0) {
		throw ToolsDownloaderException("No builds found for the version number " + versionNumber);
	}

	buildurl = versionUrl + builds.getLatest(); // use the latest build
}

bool ToolsDownloader::download(const fs::path& to)
{
	return downloadFromCore(to);
}

bool ToolsDownloader::downloadFromCore(const fs::path& to)
{
	fs::path temppath = fs::temp_directory_path();

	// No tools available, try getting them from core fusion file
	std::string coreurl = buildurl + FUSION_DEF_CORE_LOC;
	std::string core_diskpath = (temppath / FUSION_DEF_CORE_NAME).string();

	Logger::debug("Downloading core tar from " + coreurl);

	try
	{
		network.curlDownload(coreurl, core_diskpath);

		// If the core package was successfully downloaded, extract the tools from it
		Logger::debug("Extracting from .tar to temp folder ...");

		fs::path temppath = fs::temp_directory_path();

		bool success = Archive::extractTar(temppath / FUSION_DEF_CORE_NAME, FUSION_DEF_CORE_NAME_ZIP, temppath / FUSION_DEF_CORE_NAME_ZIP);
		if (!success) {
			Logger::error("Couldn't extract from the tar file");
			// Error in the tar file, try the next version number
			return false;
		}

		Logger::debug("Extracting from .zip to destination folder ...");

		success = Archive::extractZip(temppath / FUSION_DEF_CORE_NAME_ZIP, FUSION_ZIP_TOOLS_ISO, to / FUSION_ZIP_TOOLS_NAME);
		success &= Archive::extractZip(temppath / FUSION_DEF_CORE_NAME_ZIP, FUSION_ZIP_PRE15_TOOLS_ISO, to / FUSION_ZIP_PRE15_TOOLS_NAME);

		// Cleanup zip file
		fs::remove(temppath / FUSION_DEF_CORE_NAME_ZIP);

		if (!success) {
			Logger::error("Couldn't extract from the zip file"); // Error in the zip file, try the next version number
			return false;
		}

		// Cleanup tar file
		fs::remove(temppath / FUSION_DEF_CORE_NAME);
	}
	catch (const NetworkException& exc) {
		if (exc.getCode() == CURLE_HTTP_RETURNED_ERROR) {
			Logger::debug("Couldn't download tools from this version number. Trying the next one");
			return false;
		}
		else {
			throw std::runtime_error(std::string(exc.what()));
		}
	}

	return true;
}
