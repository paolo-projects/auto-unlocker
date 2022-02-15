#include "unlocker_win.h"

void applyPatchWin(const fs::path& vmwareInstallPath, const fs::path& vmwareInstallPath64)
{
	// Setup paths

	std::string binList[] = VM_WIN_PATCH_FILES;

	fs::path vmwarebase_path = vmwareInstallPath;//vmInfo.getInstallPath();
	fs::path vmx_path = vmwareInstallPath64;//vmInfo.getInstallPath64();
	fs::path vmx = vmx_path / binList[0];
	fs::path vmx_debug = vmx_path / binList[1];
	fs::path vmx_stats = vmx_path / binList[2];
	fs::path vmwarebase = vmwarebase_path / binList[3];

	if (!fs::exists(vmx))
	{
		throw std::runtime_error("Vmx file not found");
	}
	if (!fs::exists(vmx_debug))
	{
		throw std::runtime_error("Vmx-debug file not found");
	}
	if (!fs::exists(vmwarebase))
	{
		throw std::runtime_error("vmwarebase.dll file not found");
	}

	Logger::info("File: " + vmx.filename().string());
	Patcher::patchSMC(vmx, false);

	Logger::info("File: " + vmx_debug.filename().string());
	Patcher::patchSMC(vmx_debug, false);

	if (fs::exists(vmx_stats))
	{
		Logger::info("File: " + vmx_stats.filename().string());
		Patcher::patchSMC(vmx_stats, false);
	}

	Logger::info("File: " + vmwarebase.filename().string());
	Patcher::patchBase(vmwarebase);
}

void stopServices()
{
	// Stop services
	Logger::info("Stopping services...");
	auto srvcList = std::list<std::string> VM_KILL_SERVICES;
	for (auto service : srvcList)
	{
		try
		{
			ServiceStopper::StopService_s(service);
			Logger::debug("Service \"" + service + "\" stopped successfully.");
		}
		catch (const ServiceStopper::ServiceStopException& ex)
		{
			// There is no need to inform the user that the service cannot be stopped if that service does not exist in the current version.
			//logerr("Couldn't stop service \"" + service + "\", " + std::string(ex.what()));
		}
	}

	auto procList = std::list<std::string> VM_KILL_PROCESSES;
	for (auto process : procList)
	{
		try {
			if (ServiceStopper::KillProcess(process))
			{
				Logger::debug("Process \"" + process + "\" killed successfully.");
			}
			else
			{
				Logger::error("Could not kill process \"" + process + "\".");
			}
		}
		catch (const ServiceStopper::ServiceStopException& ex)
		{
			Logger::error(ex.what());
		}
	}
}

void restartServices()
{
	Logger::info("Restarting services...");
	std::vector<std::string> servicesToStart = VM_KILL_SERVICES;
	for (auto it = servicesToStart.rbegin(); it != servicesToStart.rend(); it++)
	{
		try
		{
			ServiceStopper::StartService_s(*it);
			Logger::debug("Service \"" + *it + "\" started successfully.");
		}
		catch (const ServiceStopper::ServiceStopException& ex)
		{
			// There is no need to inform the user that the service cannot be started if that service does not exist in the current version.
			//logerr("Couldn't start service " + *it);
		}
	}
}

void preparePatchWin(fs::path backupPath, fs::path vmInstallPath)
{

	stopServices();

	// Backup files
	auto filesToBackup = std::map<std::string, std::string> VM_WIN_BACKUP_FILES;

	for (auto element : filesToBackup)
	{
		auto filen = element.first;
		fs::path fPath = vmInstallPath / filen;
		fs::path destpath = backupPath / element.second;
		if (!fs::exists(destpath))
		{
			fs::create_directory(destpath);
		}

		try
		{
			if (fs::copy_file(fPath, destpath / fPath.filename(), fs::copy_options::overwrite_existing))
			{
				Logger::info("File \"" + fPath.string() + "\" backup done.");
			}
			else
			{
				Logger::error("File \"" + fPath.string() + "\" could not be copied for backup, patch uninstallation will not be possible.");
			}
		}
		catch (const std::exception& e)
		{
			Logger::error(std::string(e.what()) + ". Patch uninstallation will not be possible.");
		}
	}
}

// Other methods

// Copy tools to VMWare directory
void copyTools(fs::path toolspath, fs::path copyTo)
{
	fs::path toolsfrom = toolspath;

	try
	{
		if (fs::copy_file(toolsfrom / FUSION_ZIP_TOOLS_NAME, copyTo / FUSION_ZIP_TOOLS_NAME, fs::copy_options::overwrite_existing))
		{
			Logger::info("File \"" + (toolsfrom / FUSION_ZIP_TOOLS_NAME).string() + "\" copy done.");
		}
		else
		{
			Logger::error("File \"" + (toolsfrom / FUSION_ZIP_TOOLS_NAME).string() + "\" could not be copied.");
		}
	}
	catch (const std::exception& e)
	{
		Logger::error(e.what());
	}

	try
	{
		if (fs::copy_file(toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME, copyTo / FUSION_ZIP_PRE15_TOOLS_NAME, fs::copy_options::overwrite_existing))
		{
			Logger::info("File \"" + (toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME).string() + "\" copy done.");
		}
		else
		{
			Logger::error("File \"" + (toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME).string() + "\" could not be copied.");
		}
	}
	catch (const std::exception& e)
	{
		Logger::error(e.what());
	}
}

// Download tools into "path"
bool downloadTools(fs::path path, std::function<void(double, double, double, double)> progressCallback)
{
	Network network;

	if (progressCallback != nullptr)
	{
		network.setProgressCallback(progressCallback);
	}

	fs::path temppath = fs::temp_directory_path(); // extract files in the temp folder first

	fs::create_directory(path); // create destination directory if it doesn't exist

	std::string url = FUSION_BASE_URL;

	std::string releasesList;

	releasesList = network.curlGet(url); // get the releases HTML page

	VersionParser versionParser(releasesList); // parse HTML page to version numbers

	if (versionParser.size() == 0)
	{
		Logger::error("No Fusion versions found in Download url location.");
		return false;
	}

	bool success = false;

	// For each version number retrieve the latest build and check if tools are available
	for (auto it = versionParser.cbegin(); it != versionParser.cend(); it++)
	{
		std::string version = it->getVersionText();

		ToolsDownloader downloader(network, FUSION_BASE_URL, version);

		if (downloader.download(path)) {
			success = true;
			break;
		}
	}

	if (success) {
		Logger::info("Tools successfully downloaded!");
	}
	else {
		Logger::error("Couldn't find tools.");
	}

	return success;
}
