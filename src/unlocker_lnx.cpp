#include "unlocker_lnx.h"

void installLnx()
{
	PatchVersioner patchVersion(LNX_PATCH_VER_PATH);

	if (patchVersion.hasPatch())
	{
		Logger::error("Patch is already installed. Uninstall it first before applying it again");
		return;
	}

	// Default output path is ./tools/
	fs::path toolsdirectory = fs::path(".") / TOOLS_DOWNLOAD_FOLDER;
	// Default backup path is ./backup/
	fs::path backup = fs::path(".") / BACKUP_FOLDER;

	Logger::info("Killing services and backing up files...");
	preparePatchLnx(backup);

	Logger::info("Patching files...");

	applyPatchLnx();

	patchVersion.writePatchData();

	Logger::verbose("Written version file at " + (fs::path(LNX_PATCH_VER_PATH) / PATCH_VER_FILE).string());

	Logger::info("Downloading tools into \"" + toolsdirectory.string() + "\" directory...");

	bool alreadyHasTools = fs::exists(fs::path(".") / TOOLS_DOWNLOAD_FOLDER / FUSION_ZIP_TOOLS_NAME) && fs::exists(fs::path(".") / TOOLS_DOWNLOAD_FOLDER / FUSION_ZIP_PRE15_TOOLS_NAME);

	if (!alreadyHasTools)
	{
		downloadTools(toolsdirectory);
	}
	else {
		Logger::info("Tools have been found in the `tools` folder. Using them...\n"
			"Please check that the existing tools are working and are the most recent ones.");
	}

	Logger::info("Copying tools into program directory...");
	copyTools(toolsdirectory);

	Logger::info("Patch complete.");
}

void uninstallLnx()
{
	PatchVersioner patchVersion(LNX_PATCH_VER_PATH);

	if (!patchVersion.hasPatch())
	{
		Logger::error("Patch is not installed");
		return;
	}

	// Default output path is ./tools/
	fs::path toolsdirectory = fs::path(".") / TOOLS_DOWNLOAD_FOLDER;
	// Default backup path is ./backup/
	fs::path backup = fs::path(".") / BACKUP_FOLDER;

	fs::path vmwareDir = VM_LNX_PATH;

	Logger::info("Restoring files...");

	// Copy contents of backup/
	std::vector<std::string> lnxBins = VM_LNX_BINS;
	for (const auto& file : lnxBins)
	{
		try
		{
			if (fs::copy_file(backup / file, vmwareDir / file, fs::copy_options::overwrite_existing))
			{
				Logger::info("File \"" + (backup / file).string() + "\" restored successfully");
			}
			else
			{
				Logger::error("Error while restoring \"" + (backup / file).string() + "\".");
			}
		}
		catch (const fs::filesystem_error& ex)
		{
			Logger::error(ex.what());
		}
	}
	std::vector<std::string> vmLibCandidates = VM_LNX_LIB_CANDIDATES;
	for (const auto& lib : vmLibCandidates)
	{
		if (fs::exists(fs::path(lib).parent_path()))
		{
			try
			{
				if (fs::copy_file(backup / fs::path(lib).filename(), fs::path(lib), fs::copy_options::overwrite_existing))
				{
					Logger::info("File \"" + (backup / fs::path(lib).filename()).string() + "\" restored successfully");
				}
				else
				{
					Logger::error("Error while restoring \"" + (backup / fs::path(lib).filename()).string() + "\".");
				}
			}
			catch (const fs::filesystem_error& ex)
			{
				Logger::error(ex.what());
			}
			break;
		}
	}

	// Remove darwin*.* from InstallDir
	for (const auto& file : fs::directory_iterator(VM_LNX_ISO_DESTPATH))
	{
		if (fs::is_regular_file(file))
		{
			size_t is_drw = file.path().filename().string().find("darwin");
			if (is_drw != std::string::npos && is_drw == 0)
			{
				fs::remove(file);
			}
		}
	}

	patchVersion.removePatchVersion();
	Logger::verbose("Removed version file from " + (fs::path(LNX_PATCH_VER_PATH) / PATCH_VER_FILE).string());

	fs::remove_all(backup);
	fs::remove_all(toolsdirectory);

	Logger::info("Uninstall complete.");
}

void applyPatchLnx()
{
	fs::path vmBinPath = VM_LNX_PATH;

	std::string binList[] = VM_LNX_BINS;

	fs::path vmx = vmBinPath / binList[0];
	fs::path vmx_debug = vmBinPath / binList[1];
	fs::path vmx_stats = vmBinPath / binList[2];

	// See if first candidate is good else use second one
	std::string libCandidates[] = VM_LNX_LIB_CANDIDATES;

	bool vmxso = true;

	fs::path vmlib = libCandidates[0];
	if (!fs::exists(vmlib)) {
		vmlib = libCandidates[1];
		vmxso = false;
	}

	if (!fs::exists(vmx))
	{
		throw std::runtime_error("Vmx file not found");
	}
	if (!fs::exists(vmx_debug))
	{
		throw std::runtime_error("Vmx-debug file not found");
	}
	if (!fs::exists(vmlib))
	{
		throw std::runtime_error("Vmlib file not found");
	}

	Logger::info("File: " + vmx.filename().string());
	CHECKRES(Patcher::patchSMC(vmx, vmxso));

	Logger::info("File: " + vmx_debug.filename().string());
	CHECKRES(Patcher::patchSMC(vmx_debug, vmxso));

	if (fs::exists(vmx_stats))
	{
		Logger::info("File: " + vmx_stats.filename().string());
		CHECKRES(Patcher::patchSMC(vmx_stats, vmxso));
	}

	Logger::info("File: " + vmlib.filename().string());
	CHECKRES(Patcher::patchBase(vmlib));
}

void preparePatchLnx(fs::path backupPath)
{
	// Backup files
	std::string filesToBackup[] = VM_LNX_BACKUP_FILES;
	fs::path destpath = backupPath;

	for (auto element : filesToBackup)
	{
		fs::path fPath = element;
		if (!fs::exists(destpath))
			fs::create_directory(destpath);

		try
		{
			if (fs::copy_file(fPath, destpath / fPath.filename(), fs::copy_options::overwrite_existing))
			{
				Logger::info("File \"" + fPath.string() + "\" backup done.");
			}
			else
			{
				Logger::error("File \"" + fPath.string() + "\" could not be copied.");
			}
		}
		catch (const std::exception& e)
		{
			Logger::error(e.what());
		}
	}

	std::string libsAlternatives[] = VM_LNX_LIB_CANDIDATES;

	for (auto lib : libsAlternatives)
	{
		fs::path libpath = lib;
		if (fs::exists(libpath.parent_path()))
		{
			try
			{
				if (fs::copy_file(libpath, destpath / libpath.filename(), fs::copy_options::overwrite_existing))
				{
					Logger::info("File \"" + libpath.string() + "\" backup done.");
				}
				else
				{
					Logger::error("File \"" + libpath.string() + "\" could not be copied.");
				}

				break;
			}
			catch (const std::exception& e)
			{
				Logger::error(e.what());
			}
		}
	}
}

// Other methods

// Copy tools to VMWare directory
void copyTools(fs::path toolspath)
{
	fs::path toolsfrom = toolspath;
	fs::path copyto = VM_LNX_ISO_DESTPATH;

	try
	{
		if (fs::copy_file(toolsfrom / FUSION_ZIP_TOOLS_NAME, copyto / FUSION_ZIP_TOOLS_NAME, fs::copy_options::overwrite_existing))
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
		if (fs::copy_file(toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME, copyto / FUSION_ZIP_PRE15_TOOLS_NAME, fs::copy_options::overwrite_existing))
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
bool downloadTools(fs::path path)
{
	Network network;

	ToolsDownloader downloader(network, FUSION_BASE_URL, version);

	bool success = downloader.download(path);

	if (success) {
		Logger::info("Tools successfully downloaded!");
	}
	else {
		Logger::error("Couldn't find tools.");
	}

	return success;
}
