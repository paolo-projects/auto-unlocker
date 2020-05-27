/************************************************************************************************
	Unlocker - Patcher + Tools Downloader
	Created by Paolo Infante

	Based on "Unlocker" by DrDonk for a native solution to python errors and 
	virus warnings.

	I coded from scratch all the patch routines (I tried to be consistent with C++ library usage
	though you'll probably find a few memcpy here and there...) and tools download (using 
	libcurl for download and libarchive to uncompress the archives)

	Should be cross-platform. Tested on Windows, I'm planning to test it on linux too
	I tried my best to use STL whenever possible. I had to use external libraries:
	libcurl, libarchive and zlib that I'm sure can be compiled on linux, though.

	To ensure cross-platform fs access I used std::filesystem from c++17, so a compiler that
	supports it is required.

	If the case arises that I'm not able to use STL I'll try to find a multiplatform solution
	or code different solutions for both win and linux (as I did for Registry access)

*************************************************************************************************/

#include <string>
#include "filesystem.hpp"
#include <map>

#include <curl/curl.h>

#include "config.h"
#include "netutils.h"
#include "versionparser.h"
#include "debugutils.h"
#include "buildsparser.h"
#include "archiveutils.h"
#include "installinfoutils.h"
#include "servicestoputils.h"
#include "patchutils.h"

#ifdef __linux__
#include <unistd.h>
#include <strings.h>

#define stricmp(a, b) strcasecmp(a, b)
#endif

#include <stdio.h>

#define CHECKRES(x) try{ (x); } catch (const Patcher::PatchException& exc) { logerr(exc.what()); }
#define KILL(x) (x); exit(1);

// Forward declarations

void preparePatch(fs::path backupPath);
void doPatch();
void downloadTools(fs::path path);
void copyTools(fs::path toolspath);
void stopServices();
void restartServices();

void install();
void uninstall();
void showhelp();

// Main function

int main(int argc, const char* argv[])
{
	std::cout << "auto-unlocker " << PROG_VERSION << std::endl
		<< std::endl;
#ifdef __linux__
	if (geteuid() != 0)
	{
		// User not root and not elevated permissions
		logd("The program is not running as root, the patch may not work properly.");
		std::cout << "Running the program with sudo/as root is recommended, in most cases required... Do you want to continue? (y/N) ";
		char c = getc(stdin);
		
		if (c != 'y' && c != 'Y')
		{
			logd("Aborting...");
			exit(0);
		}
	}
#endif
	if (argc > 1)
	{
		const char* arg = argv[1];

		if (stricmp(arg, UNINSTALL_OPTION) == 0)
			uninstall();
		else if (stricmp(arg, HELP_OPTION) == 0)
			showhelp();
		else if (stricmp(arg, INSTALL_OPTION) == 0)
			install();
		else
		{
			logd("Unrecognized command.");
			logd("");
			showhelp();
		}
	}
	else {
		fs::path backupFolder = BACKUP_FOLDER;
		if (fs::exists(backupFolder))
		{
			std::cout << "A backup folder has been found. Do you wish to uninstall the previous patch? Type y to uninstall, n to continue with installation." << std::endl
				<< "(Y/n) ";
			
			char c = getc(stdin);

			if (c == 'n' || c == 'N')
				install();
			else
				uninstall();
		} else install();
	}

#ifdef _WIN32
	logd("Press enter to quit.");
	getc(stdin);
#endif

	return 0;
}

void install()
{
	// Default output path is ./tools/
	fs::path toolsdirectory = fs::path(".") / TOOLS_DOWNLOAD_FOLDER;
	// Default backup path is ./backup/
	fs::path backup = fs::path(".") / BACKUP_FOLDER;

	logd("Killing services and backing up files...");
	preparePatch(backup);

	logd("Patching files...");
	doPatch();

	logd("Downloading tools into \"" + toolsdirectory.string() + "\" directory...");

	if (fs::exists(fs::path(".") / TOOLS_DOWNLOAD_FOLDER / FUSION_ZIP_TOOLS_NAME) && fs::exists(fs::path(".") / TOOLS_DOWNLOAD_FOLDER / FUSION_ZIP_PRE15_TOOLS_NAME))
	{
		std::cout << "Tools have been found in the executable folder. Do you want to use the existing tools instead of downloading them again?" << std::endl
			<< "Please check that the existing tools are working and are the most recent ones." << std::endl
			<< "(Y/n) ";

		char c = getc(stdin);

		if (c != 'y' && c != 'Y')
			downloadTools(toolsdirectory);
	}
	else
		downloadTools(toolsdirectory);

	logd("Copying tools into program directory...");
	copyTools(toolsdirectory);

	restartServices();

	logd("Patch complete.");
}

void uninstall()
{
#ifdef _WIN32
	VMWareInfoRetriever vmInfo;

	// Stop services
	stopServices();

	// Default output path is ./tools/
	fs::path toolsdirectory = fs::path(".") / TOOLS_DOWNLOAD_FOLDER;
	// Default backup path is ./backup/
	fs::path backup = fs::path(".") / BACKUP_FOLDER;

	fs::path vmwareInstallDir = vmInfo.getInstallPath();
	fs::path vmwareInstallDir64 = vmInfo.getInstallPath64();

	logd("Restoring files...");
	// Copy contents of backup/
	if (fs::exists(backup))
	{
		for (const auto& file : fs::directory_iterator(backup))
		{
			if (fs::is_regular_file(file))
			{
				try
				{
					if (fs::copy_file(file.path(), vmwareInstallDir / file.path().filename(), fs::copy_options::overwrite_existing))
						logd("File \"" + file.path().string() + "\" restored successfully");
					else
						logerr("Error while restoring \"" + file.path().string() + "\".");
				}
				catch (fs::filesystem_error ex)
				{
					logerr(ex.what());
				}
			}
		}
		// Copy contents of backup/x64/
		for (const auto& file : fs::directory_iterator(backup / "x64"))
		{
			if (fs::is_regular_file(file))
			{
				try
				{
					if (fs::copy_file(file.path(), vmwareInstallDir64 / file.path().filename(), fs::copy_options::overwrite_existing))
						logd("File \"" + file.path().string() + "\" restored successfully");
					else
						logerr("Error while restoring \"" + file.path().string() + "\".");
				}
				catch (fs::filesystem_error ex)
				{
					logerr(ex.what());
				}
			}
		}
	}
	else {
		logerr("Couldn't find backup files...");
		return;
	}
	// Remove darwin*.* from InstallDir
	for (const auto& file : fs::directory_iterator(vmwareInstallDir))
	{
		if (fs::is_regular_file(file))
		{
			size_t is_drw = file.path().filename().string().find("darwin");
			if (is_drw != std::string::npos && is_drw == 0)
				fs::remove(file);
		}
	}

	fs::remove_all(backup);
	fs::remove_all(toolsdirectory);

	// Restart services
	restartServices();

	logd("Uninstall complete.");
#elif defined (__linux__)
	// Default output path is ./tools/
	fs::path toolsdirectory = fs::path(".") / TOOLS_DOWNLOAD_FOLDER;
	// Default backup path is ./backup/
	fs::path backup = fs::path(".") / BACKUP_FOLDER;

	fs::path vmwareDir = VM_LNX_PATH;

	logd("Restoring files...");

	// Copy contents of backup/
	std::vector<std::string> lnxBins = VM_LNX_BINS;
	for (const auto& file : lnxBins)
	{
		try
		{
			if (fs::copy_file(backup / file, vmwareDir / file, fs::copy_options::overwrite_existing))
				logd("File \"" + (backup / file).string() + "\" restored successfully");
			else
				logerr("Error while restoring \"" + (backup / file).string() + "\".");
		}
		catch (fs::filesystem_error ex)
		{
			logerr(ex.what());
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
					logd("File \"" + (backup / fs::path(lib).filename()).string() + "\" restored successfully");
				else
					logerr("Error while restoring \"" + (backup / fs::path(lib).filename()).string() + "\".");
			}
			catch (fs::filesystem_error ex)
			{
				logerr(ex.what());
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
				fs::remove(file);
		}
	}

	fs::remove_all(backup);
	fs::remove_all(toolsdirectory);

	logd("Uninstall complete.");
#endif
}

void showhelp()
{
	std::cout << "auto-unlocker" << std::endl << std::endl
		<< "Run the program with one of these options:" << std::endl
		<< "	--install (default): install the patch" << std::endl
		<< "	--uninstall: remove the patch" << std::endl
		<< "	--help: show this help message" << std::endl;
}

// Other methods

// Copy tools to VMWare directory
void copyTools(fs::path toolspath)
{
#ifdef _WIN32
	VMWareInfoRetriever vmInfo;
	fs::path copyto = vmInfo.getInstallPath();
	fs::path toolsfrom = toolspath;

	try
	{
		if (fs::copy_file(toolsfrom / FUSION_ZIP_TOOLS_NAME, copyto / FUSION_ZIP_TOOLS_NAME))
			logd("File \"" + (toolsfrom / FUSION_ZIP_TOOLS_NAME).string() + "\" copy done.");
		else
			logerr("File \"" + (toolsfrom / FUSION_ZIP_TOOLS_NAME).string() + "\" could not be copied.");
	}
	catch (const std::exception & e)
	{
		logerr(e.what());
	}
	
	try
	{
		if (fs::copy_file(toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME, copyto / FUSION_ZIP_PRE15_TOOLS_NAME))
			logd("File \"" + (toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME).string() + "\" copy done.");
		else
			logerr("File \"" + (toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME).string() + "\" could not be copied.");
	}
	catch (const std::exception & e)
	{
		logerr(e.what());
	}
#elif defined (__linux__)
	fs::path copyto = VM_LNX_ISO_DESTPATH;
	fs::path toolsfrom = toolspath;

	try
	{
		if (fs::copy_file(toolsfrom / FUSION_ZIP_TOOLS_NAME, copyto / FUSION_ZIP_TOOLS_NAME))
			logd("File \"" + (toolsfrom / FUSION_ZIP_TOOLS_NAME).string() + "\" copy done.");
		else
			logerr("File \"" + (toolsfrom / FUSION_ZIP_TOOLS_NAME).string() + "\" could not be copied.");
	}
	catch (const std::exception & e)
	{
		logerr(e.what());
	}

	try
	{
		if (fs::copy_file(toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME, copyto / FUSION_ZIP_PRE15_TOOLS_NAME))
			logd("File \"" + (toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME).string() + "\" copy done.");
		else
			logerr("File \"" + (toolsfrom / FUSION_ZIP_PRE15_TOOLS_NAME).string() + "\" could not be copied.");
	}
	catch (const std::exception & e)
	{
		logerr(e.what());
	}
#endif
}

// Actual patch code
void doPatch()
{
#ifdef _WIN32
	// Setup paths
	VMWareInfoRetriever vmInfo;
	
	std::string binList[] = VM_WIN_PATCH_FILES;

	fs::path vmwarebase_path = vmInfo.getInstallPath();
	fs::path vmx_path = vmInfo.getInstallPath64();
	fs::path vmx = vmx_path / binList[0];
	fs::path vmx_debug = vmx_path / binList[1];
	fs::path vmx_stats = vmx_path / binList[2];
	fs::path vmwarebase = vmwarebase_path / binList[3];

	if(!fs::exists(vmx))
	{
		KILL(logerr("Vmx file not found"));
	}
	if(!fs::exists(vmx_debug))
	{
		KILL(logerr("Vmx file not found"));
	}
	if(!fs::exists(vmwarebase))
	{
		KILL(logerr("Vmx file not found"));
	}

	logd("File: " + vmx.filename().string());
	CHECKRES(Patcher::patchSMC(vmx, false));

	logd("File: " + vmx_debug.filename().string());
	CHECKRES(Patcher::patchSMC(vmx_debug, false));

	if (fs::exists(vmx_stats))
	{
		logd("File: " + vmx_stats.filename().string());
		CHECKRES(Patcher::patchSMC(vmx_stats, false));
	}

	logd("File: " + vmwarebase.filename().string());
	CHECKRES(Patcher::patchBase(vmwarebase));

#elif defined (__linux__)
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

        if(!fs::exists(vmx))
	{
		KILL(logerr("Vmx file not found"));
	}
        if(!fs::exists(vmx_debug))
	{
		KILL(logerr("Vmx-debug file not found"));
	}
	if(!fs::exists(vmlib))
	{
		KILL(logerr("Vmlib file not found"));
	}

	logd("File: " + vmx.filename().string());
	CHECKRES(Patcher::patchSMC(vmx, vmxso));

	logd("File: " + vmx_debug.filename().string());
	CHECKRES(Patcher::patchSMC(vmx_debug, vmxso));

	if (fs::exists(vmx_stats))
	{
		logd("File: " + vmx_stats.filename().string());
		CHECKRES(Patcher::patchSMC(vmx_stats, vmxso));
	}

	logd("File: " + vmlib.filename().string());
	CHECKRES(Patcher::patchBase(vmlib));
#else
	logerr("OS not supported");
	exit(1);
#endif
}

void stopServices()
{
#ifdef _WIN32
	// Stop services
	auto srvcList = std::list<std::string> VM_KILL_SERVICES;
	for (auto service : srvcList)
	{
		try
		{
			ServiceStopper::StopService_s(service);
			logd("Service \"" + service + "\" stopped successfully.");
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
				logd("Process \"" + process + "\" killed successfully.");
			else
				logerr("Could not kill process \"" + process + "\".");
		}
		catch (const ServiceStopper::ServiceStopException & ex)
		{
			logerr(ex.what());
		}
	}
#endif
}

void restartServices()
{
#ifdef _WIN32
	logd("Restarting services...");
	std::vector<std::string> servicesToStart = VM_KILL_SERVICES;
	for (auto it = servicesToStart.rbegin(); it != servicesToStart.rend(); it++)
	{
		try
		{
			ServiceStopper::StartService_s(*it);
			logd("Service \"" + *it + "\" started successfully.");
		}
		catch (const ServiceStopper::ServiceStopException & ex)
		{
			// There is no need to inform the user that the service cannot be started if that service does not exist in the current version.
			//logerr("Couldn't start service " + *it);
		}
	}
#endif
}

// Kill services / processes and backup files
void preparePatch(fs::path backupPath)
{
#ifdef _WIN32
	// Retrieve installation path from registry
	VMWareInfoRetriever vmInstall;

	stopServices();

	// Backup files
	auto filesToBackup = std::map<std::string, std::string> VM_WIN_BACKUP_FILES;

	for (auto element : filesToBackup)
	{
		auto filen = element.first;
		fs::path fPath = (vmInstall.getInstallPath() + filen);
		fs::path destpath = backupPath / element.second;
		if (!fs::exists(destpath))
			fs::create_directory(destpath);

		try
		{
			if (fs::copy_file(fPath, destpath / fPath.filename(), fs::copy_options::overwrite_existing))
				logd("File \"" + fPath.string() + "\" backup done.");
			else
				logerr("File \"" + fPath.string() + "\" could not be copied.");
		}
		catch (const std::exception& e)
		{
			logerr(e.what());
		}
	}
#elif defined (__linux__)
	//TODO: linux code here
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
				logd("File \"" + fPath.string() + "\" backup done.");
			else
				logerr("File \"" + fPath.string() + "\" could not be copied.");
		}
		catch (const std::exception & e)
		{
			logerr(e.what());
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
					logd("File \"" + libpath.string() + "\" backup done.");
				else
					logerr("File \"" + libpath.string() + "\" could not be copied.");

				break;
			}
			catch (const std::exception & e)
			{
				logerr(e.what());
			}
		}
	}
#else
	// Either the compiler macros are not working or the the tool is trying to be compiled on an OS where it's not meant to be compiled
	logerr("OS not supported");
	exit(1); // Better stop before messing things up
#endif
}

// Download tools into "path"
void downloadTools(fs::path path)
{
	fs::path temppath = fs::temp_directory_path(); // extract files in the temp folder first

	fs::create_directory(path); // create destination directory if it doesn't exist
	
	curl_global_init(CURL_GLOBAL_ALL);

	std::string url = FUSION_BASE_URL;

	std::string releasesList;
	Curl::curlGet(url, releasesList); // get the releases HTML page

	VersionParser versionParser(releasesList); // parse HTML page to version numbers

	if (versionParser.size() == 0)
	{
		logerr("No Fusion versions found in Download url location.");
		return;
	}

	bool success = false;

	// For each version number retrieve the latest build and check if tools are available
	for (auto it = versionParser.cbegin(); it != versionParser.cend(); it++)
	{
		std::string version = it->getVersionText();

		std::string versionurl = url + version + "/";
		std::string versionhtml;

		Curl::curlGet(versionurl, versionhtml);

		BuildsParser builds(versionhtml); // parse the builds in the page

		if (builds.size() > 0)
		{
			std::string buildurl = versionurl + builds.getLatest(); // use the latest build

			std::string toolsurl = buildurl + FUSION_DEF_TOOLS_LOC;
			std::string tools_pre15_url = buildurl + FUSION_DEF_PRE15_TOOLS_LOC;

			std::string tools_diskpath = (temppath / FUSION_DEF_TOOLS_NAME).string();
			std::string toolspre15_diskpath = (temppath / FUSION_DEF_PRE15_TOOLS_NAME).string();


			bool toolsAvailable = (Curl::curlDownload(toolsurl, tools_diskpath) == CURLE_OK);
			toolsAvailable &= (Curl::curlDownload(tools_pre15_url, toolspre15_diskpath) == CURLE_OK);

			if (toolsAvailable) // if tools were successfully downloaded, extract them to destination folder
			{
				success = Archive::extract_s(temppath / FUSION_DEF_TOOLS_NAME, FUSION_DEF_TOOLS_ZIP, temppath / FUSION_DEF_TOOLS_ZIP);
				success &= Archive::extract_s(temppath / FUSION_DEF_PRE15_TOOLS_NAME, FUSION_DEF_PRE15_TOOLS_ZIP, temppath / FUSION_DEF_PRE15_TOOLS_ZIP);

				if (!success)
				{
					logerr("Couldn't extract zip files from tars");
					continue;
				}

				success = Archive::extract_s(temppath / FUSION_DEF_TOOLS_ZIP, FUSION_TAR_TOOLS_ISO, path / FUSION_ZIP_TOOLS_NAME);
				success &= Archive::extract_s(temppath / FUSION_DEF_PRE15_TOOLS_ZIP, FUSION_TAR_PRE15_TOOLS_ISO, path / FUSION_ZIP_PRE15_TOOLS_NAME);

				// Cleanup zips
				fs::remove(temppath / FUSION_DEF_TOOLS_ZIP);
				fs::remove(temppath / FUSION_DEF_PRE15_TOOLS_ZIP);

				if (!success)
				{
					logerr("Couldn't extract tools from zip files");
				}
				else
				{
					// Cleanup tars
					fs::remove(temppath / FUSION_DEF_TOOLS_NAME);
					fs::remove(temppath / FUSION_DEF_PRE15_TOOLS_NAME);

					success = true;
					break;
				}
			}
			else {
				// No tools available, try getting them from core fusion file
				std::string coreurl = buildurl + FUSION_DEF_CORE_LOC;
				std::string core_diskpath = (temppath / FUSION_DEF_CORE_NAME).string();
				
				if (Curl::curlDownload(coreurl, core_diskpath) == CURLE_OK) // If the core package was successfully downloaded, extract the tools from it
				{
					logd("Extracting from .tar to temp folder ...");
					
					fs::path temppath = fs::temp_directory_path();

					success = Archive::extract_s(temppath/FUSION_DEF_CORE_NAME, FUSION_DEF_CORE_NAME_ZIP, temppath/FUSION_DEF_CORE_NAME_ZIP);
					if (!success) {
						logerr("Couldn't extract from the tar file");
						// Error in the tar file, try the next version number
						continue;
					}

					logd("Extracting from .zip to destination folder ...");

					success = Archive::extract_s(temppath / FUSION_DEF_CORE_NAME_ZIP, FUSION_ZIP_TOOLS_ISO, path / FUSION_ZIP_TOOLS_NAME);
					success = Archive::extract_s(temppath / FUSION_DEF_CORE_NAME_ZIP, FUSION_ZIP_PRE15_TOOLS_ISO, path / FUSION_ZIP_PRE15_TOOLS_NAME);

					// Cleanup zip file
					fs::remove(temppath / FUSION_DEF_CORE_NAME_ZIP);

					if (!success)
						logerr("Couldn't extract from the zip file"); // Error in the zip file, try the next version number
					else
					{
						// Cleanup tar file
						fs::remove(temppath / FUSION_DEF_CORE_NAME);

						break; // All went good
					}
				}

				// Cleanup tar file
				fs::remove(temppath / FUSION_DEF_CORE_NAME);
			}

			// Cleanup tars
			fs::remove(temppath / FUSION_DEF_TOOLS_NAME);
			fs::remove(temppath / FUSION_DEF_PRE15_TOOLS_NAME);
		}
	}

	if (success) {
		logd("Tools successfully downloaded!");
	}
	else {
		logerr("Couldn't find tools.");
	}

	curl_global_cleanup();
}
