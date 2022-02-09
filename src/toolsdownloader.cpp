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
	bool success = downloadStandalone(to);
	if (!success) {
		success = downloadFromCore(to);
	}
	return success;
}

bool ToolsDownloader::downloadStandalone(const fs::path& to)
{
	fs::path temppath = fs::temp_directory_path();

	std::string toolsurl = buildurl + FUSION_DEF_TOOLS_LOC;
	std::string tools_pre15_url = buildurl + FUSION_DEF_PRE15_TOOLS_LOC;

	std::string tools_diskpath = (temppath / FUSION_DEF_TOOLS_NAME).string();
	std::string toolspre15_diskpath = (temppath / FUSION_DEF_PRE15_TOOLS_NAME).string();

	logd("Downloading tools from " + toolsurl + " and " + tools_pre15_url);

	try {
		network.curlDownload(toolsurl, tools_diskpath);
		network.curlDownload(tools_pre15_url, toolspre15_diskpath);

		// if tools were successfully downloaded, extract them to destination folder
		bool success = Archive::extractTar(temppath / FUSION_DEF_TOOLS_NAME, FUSION_DEF_TOOLS_ZIP, temppath / FUSION_DEF_TOOLS_ZIP);
		success &= Archive::extractTar(temppath / FUSION_DEF_PRE15_TOOLS_NAME, FUSION_DEF_PRE15_TOOLS_ZIP, temppath / FUSION_DEF_PRE15_TOOLS_ZIP);

		if (!success)
		{
			logerr("Couldn't extract zip files from tars");
			return false;
		}

		success = Archive::extractZip(temppath / FUSION_DEF_TOOLS_ZIP, FUSION_TAR_TOOLS_ISO, to / FUSION_ZIP_TOOLS_NAME);
		success &= Archive::extractZip(temppath / FUSION_DEF_PRE15_TOOLS_ZIP, FUSION_TAR_PRE15_TOOLS_ISO, to / FUSION_ZIP_PRE15_TOOLS_NAME);

		// Cleanup zips
		fs::remove(temppath / FUSION_DEF_TOOLS_ZIP);
		fs::remove(temppath / FUSION_DEF_PRE15_TOOLS_ZIP);

		if (!success)
		{
			logerr("Couldn't extract tools from zip files");
			return false;
		}

		// Cleanup tars
		fs::remove(temppath / FUSION_DEF_TOOLS_NAME);
		fs::remove(temppath / FUSION_DEF_PRE15_TOOLS_NAME);
	}
	catch (const NetworkException& exc) {
		if (exc.getCode() == CURLE_HTTP_RETURNED_ERROR) {
			logd("Tools not found as standalone");
			return false;
		}
		else {
			throw std::runtime_error(std::string(exc.what()));
		}
	}

	return true;
}

bool ToolsDownloader::downloadFromCore(const fs::path& to)
{
	fs::path temppath = fs::temp_directory_path();

	// No tools available, try getting them from core fusion file
	std::string coreurl = buildurl + FUSION_DEF_CORE_LOC;
	std::string core_diskpath = (temppath / FUSION_DEF_CORE_NAME).string();

	logd("Downloading core tar from " + coreurl);

	try
	{
		network.curlDownload(coreurl, core_diskpath);

		// If the core package was successfully downloaded, extract the tools from it
		logd("Extracting from .tar to temp folder ...");

		fs::path temppath = fs::temp_directory_path();

		bool success = Archive::extractTar(temppath / FUSION_DEF_CORE_NAME, FUSION_DEF_CORE_NAME_ZIP, temppath / FUSION_DEF_CORE_NAME_ZIP);
		if (!success) {
			logerr("Couldn't extract from the tar file");
			// Error in the tar file, try the next version number
			return false;
		}

		logd("Extracting from .zip to destination folder ...");

		success = Archive::extractZip(temppath / FUSION_DEF_CORE_NAME_ZIP, FUSION_ZIP_TOOLS_ISO, to / FUSION_ZIP_TOOLS_NAME);
		success &= Archive::extractZip(temppath / FUSION_DEF_CORE_NAME_ZIP, FUSION_ZIP_PRE15_TOOLS_ISO, to / FUSION_ZIP_PRE15_TOOLS_NAME);

		// Cleanup zip file
		fs::remove(temppath / FUSION_DEF_CORE_NAME_ZIP);

		if (!success) {
			logerr("Couldn't extract from the zip file"); // Error in the zip file, try the next version number
			return false;
		}

		// Cleanup tar file
		fs::remove(temppath / FUSION_DEF_CORE_NAME);
	}
	catch (const NetworkException& exc) {
		if (exc.getCode() == CURLE_HTTP_RETURNED_ERROR) {
			logd("Couldn't download tools from this version number. Trying the next one");
			return false;
		}
		else {
			throw std::runtime_error(std::string(exc.what()));
		}
	}

	return true;
}
