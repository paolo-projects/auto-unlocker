/************************************************************************************************
	Unlocker - Patcher + Tools Downloader
	Created by Paolo Infante - Based on "Unlocker" by DrDonk

	This project is born to provide a native solution, mainly on Windows, to the original
	Unlocker project. Since the original project requires python, and it's not installed
	by default on windows, this tool has been coded to provide an all-in-one solution with
	no additional dependencies needed.

	It relies on libcurl for the networking (get requests and file download) and libzip
	for the zip extraction. The tar extraction, being relatively easy to implement, has been coded from
	scratch. The libraries are linked statically to provide one final executable for the sake
	of simplicity.

	The future of this project is to provide a GUI for the windows target using the Win32 API.
	Although this solution brings more complexity than using one of the many GUI libraries out there
	such as MFC, QT, GTK, wx, the resulting executable will be smaller and with better compatibility.

	For the linux target, it will still be shell-based, given the better-instructed user base running
	this tool on linux os-es.

*************************************************************************************************/

#ifdef _WIN32
#include <Windows.h>
#include "win32/mainwindow.h"

void loadVisualStyles();
bool registerControlStyles(DWORD style);
#endif

#include <iostream>
#include "config.h"

#ifdef __linux__
#include "unlocker_lnx.h"
#include "logging/terminallogstrategy.h"

void showhelp()
{
	std::cout << "auto-unlocker " << PROG_VERSION << std::endl << std::endl
		<< "Run the program with one of these options:" << std::endl
		<< "	--install (default): install the patch" << std::endl
		<< "	--uninstall: remove the patch" << std::endl
		<< "	--download-tools: only download the tools" << std::endl
		<< "	--help: show this help message" << std::endl;
}

int main(int argc, const char* argv[])
{
	TerminalLogStrategy logStrategy;
	Logger::init(&logStrategy);

	std::cout << "auto-unlocker " << PROG_VERSION << std::endl
		<< std::endl;

	try {
#ifdef __linux__
		if (geteuid() != 0)
		{
			// User not root and not elevated permissions
			std::cout << "The program is not running as root, the patch may not work properly." << std::endl <<
						"Running the program with sudo/as root is recommended, in most cases required... Do you want to continue? (y/n) ";
			std::string c;
			std::cin >> c;

			if (c != "y" && c != "Y")
			{
				std::cout << "Aborting..." << std::endl;
				exit(0);
			}
		}
#endif
		if (argc > 1)
		{
			const char* arg = argv[1];

			if (stricmp(arg, UNINSTALL_OPTION) == 0)
				uninstallLnx();
			else if (stricmp(arg, HELP_OPTION) == 0)
				showhelp();
			else if (stricmp(arg, INSTALL_OPTION) == 0)
				installLnx();
			else if (stricmp(arg, DOWNLOADONLY_OPTION) == 0)
				downloadTools(fs::path(".") / TOOLS_DOWNLOAD_FOLDER);
			else
			{
				std::cout << "Unrecognized command." << std::endl << std::endl;
				showhelp();
			}
		}
		else {
			fs::path backupFolder = BACKUP_FOLDER;
			if (fs::exists(backupFolder))
			{
				std::cout << "A backup folder has been found. Do you wish to uninstall the previous patch? Type y to uninstall, n to continue with installation." << std::endl
					<< "(y/n) ";

				std::string c;
				std::cin >> c;

				if (c == "n" || c == "N")
					installLnx();
				else
					uninstallLnx();
			}
			else installLnx();
		}
	}
	catch (const std::exception& exc)
	{
		Logger::error(std::string(exc.what()));
	}

	Logger::free();
	return 0;
}
#elif defined _WIN32

bool registerControlStyles(DWORD style)
{
	INITCOMMONCONTROLSEX comButton = {};
	comButton.dwSize = sizeof(comButton);
	comButton.dwICC = style;
	return InitCommonControlsEx(&comButton);
}

void loadVisualStyles()
{
	std::vector<DWORD> styles = {
		ICC_WIN95_CLASSES,
		ICC_PROGRESS_CLASS
	};

	for (DWORD style : styles)
	{
		registerControlStyles(style);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevINSTANCE, PSTR pCmdLine, int nCmdShow)
{
	MainWindow mainWindow(hInstance, nCmdShow);
	mainWindow.show();
}

#endif