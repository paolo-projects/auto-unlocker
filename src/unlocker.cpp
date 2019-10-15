/************************************************************************************************
	Unlocker - Patcher + Tools Downloader
	Created by Paolo Infante

	A based on "Unlocker" by DrDonk for a native solution to python errors and 
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
#include <filesystem>
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
#endif

#define CHECKRES(x) if(!(x)) logerr("Couldn't patch file.")

namespace fs = std::filesystem;

// Forward declarations

void preparePatch(fs::path backupPath);
void doPatch();
void downloadTools(std::string path);

// Main function

int main(int argc, const char* argv[])
{
#ifdef __linux__
	if (geteuid() != 0)
	{
		// User not root and not elevated permissions
		logd("The program is not running as root, the patch may not work properly.");
		std::cout << "Running the program with sudo is suggested... Do you want to continue? (y/N) ";
		char c;
		std::cin >> c;
		if (c != 'y' && c != 'Y')
		{
			logd("Aborting...");
			exit(0);
		}
	}
#endif
	// Default output path is ./tools/
	std::string directory = (fs::path(".") / "tools" / "").string();
	// Default backup path is ./backup/
	fs::path backup = fs::path(".") / "backup";

	if (argc > 1)
	{
		// First argument is the output path
		directory = std::string(argv[1]);
	}

	if (argc > 2)
	{
		// Second argument is the backup path
		backup = argv[2];
	}

	logd("Killing services and backing up files...");
	preparePatch(backup);

	logd("Patching files...");
	doPatch();

	logd("Downloading tools into \""+ directory +"\" directory...");
	downloadTools(directory);
	
	return 0;
}

// Other methods

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

#elif defined (__linux)
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

// Kill services / processes and backup files
void preparePatch(fs::path backupPath)
{
#ifdef _WIN32
	// Retrieve installation path from registry
	VMWareInfoRetriever vmInstall;
	// Stop services
	auto srvcList = std::list<std::string> VM_KILL_SERVICES;
	for (auto service : srvcList)
	{
		try
		{
			ServiceStopper::StopService_s(service);
			logd("Service \"" + service + "\" stopped successfully.");
		}
		catch (const ServiceStopper::ServiceStopException ex)
		{
			logerr("Couldn't stop service \"" + service + "\", " + std::string(ex.what()));
		}

	}

	auto procList = std::list<std::string> VM_KILL_PROCESSES;
	for (auto process : procList)
	{
		if (ServiceStopper::KillProcess(process))
			logd("Process \"" + process + "\" killed successfully.");
		else
			logerr("Could not kill process \"" + process + "\".");
	}

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
#else
	// Either the compiler macros are not working or the the tool is trying to be compiled on an OS where it's not meant to be compiled
	logerr("OS not supported");
	exit(1); // Better stop before messing things up
#endif
}

// Download tools into "path"
void downloadTools(std::string path)
{
	fs::path temppath = fs::temp_directory_path(); // extract files in the temp folder first

	fs::create_directory(path); // create destination directory if it doesn't exist
	
	curl_global_init(CURL_GLOBAL_ALL);

	std::string url = FUSION_BASE_URL;

	std::string releasesList;
	curlGet(url, releasesList); // get the releases HTML page

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

		curlGet(versionurl, versionhtml);

		BuildsParser builds(versionhtml); // parse the builds in the page

		if (builds.size() > 0)
		{
			std::string buildurl = versionurl + builds.getLatest(); // use the latest build

			std::string toolsurl = buildurl + FUSION_DEF_TOOLS_LOC;
			std::string tools_pre15_url = buildurl + FUSION_DEF_PRE15_TOOLS_LOC;

			std::string tools_diskpath = (temppath / FUSION_DEF_TOOLS_NAME).string();
			std::string toolspre15_diskpath = (temppath / FUSION_DEF_PRE15_TOOLS_NAME).string();


			bool toolsAvailable = (curlDownload(toolsurl, tools_diskpath) == CURLE_OK);
			toolsAvailable &= (curlDownload(tools_pre15_url, toolspre15_diskpath) == CURLE_OK);

			if (toolsAvailable) // if tools were successfully downloaded, extract them to destination folder
			{
				//TODO: extract zips from tar files, and then darwin.iso and darwinPre15.iso from zip file to destination
				success = extract_s(temppath / FUSION_DEF_TOOLS_NAME, FUSION_DEF_TOOLS_ZIP, temppath / FUSION_DEF_TOOLS_ZIP);
				success &= extract_s(temppath / FUSION_DEF_PRE15_TOOLS_NAME, FUSION_DEF_PRE15_TOOLS_ZIP, temppath / FUSION_DEF_PRE15_TOOLS_ZIP);

				if (!success)
				{
					logerr("Couldn't extract zip files from tars");
					continue;
				}

				success = extract_s(temppath / FUSION_DEF_TOOLS_ZIP, FUSION_TAR_TOOLS_ISO, path + FUSION_ZIP_TOOLS_NAME);
				success &= extract_s(temppath / FUSION_DEF_PRE15_TOOLS_ZIP, FUSION_TAR_PRE15_TOOLS_ISO, path + FUSION_ZIP_PRE15_TOOLS_NAME);

				if (!success)
				{
					logerr("Couldn't extract tools from zip files");
					continue;
				}

				// Cleanup zips
				fs::remove(temppath / FUSION_DEF_TOOLS_ZIP);
				fs::remove(temppath / FUSION_DEF_PRE15_TOOLS_ZIP);

				success = true;
				break;
			}
			else {
				// No tools available, try getting them from core fusion file
				std::string coreurl = buildurl + FUSION_DEF_CORE_LOC;
				std::string core_diskpath = (temppath / FUSION_DEF_CORE_NAME).string();
				
				if (curlDownload(coreurl, core_diskpath) == CURLE_OK) // If the core package was successfully downloaded, extract the tools from it
				{
					logd("Extracting from .tar to temp folder ...");
					
					fs::path temppath = fs::temp_directory_path();

					success = extract_s(temppath/FUSION_DEF_CORE_NAME, temppath.string(), temppath/FUSION_DEF_CORE_NAME_ZIP);
					if (!success) {
						logerr("Couldn't extract from the tar file");
						// Error in the tar file, try the next version number
						continue;
					}

					logd("Extracting from .zip to destination folder ...");

					success = extract_s(temppath / FUSION_DEF_CORE_NAME_ZIP, FUSION_ZIP_TOOLS_ISO, path + FUSION_ZIP_TOOLS_NAME);
					success = extract_s(temppath / FUSION_DEF_CORE_NAME_ZIP, FUSION_ZIP_PRE15_TOOLS_ISO, path + FUSION_ZIP_PRE15_TOOLS_NAME);

					// Cleanup zip file
					fs::remove(temppath / FUSION_DEF_CORE_NAME_ZIP);

					if (!success)
						logerr("Couldn't extract from the zip file"); // Error in the zip file, try the next version number
					else
						break; // All went good
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