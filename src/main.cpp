#include <iostream>
#include "unlocker.h"

int main(int argc, const char* argv[])
{
	std::cout << "auto-unlocker " << PROG_VERSION << std::endl
		<< std::endl;

	try {
#ifdef __linux__
		if (geteuid() != 0)
		{
			// User not root and not elevated permissions
			std::cout << "The program is not running as root, the patch may not work properly." << std::endl <<
						"Running the program with sudo/as root is recommended, in most cases required... Do you want to continue? (y/N) ";
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
				uninstall();
			else if (stricmp(arg, HELP_OPTION) == 0)
				showhelp();
			else if (stricmp(arg, INSTALL_OPTION) == 0)
				install();
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
					install();
				else
					uninstall();
			}
			else install();
		}
	}
	catch (const std::exception& exc)
	{
		logerr(std::string(exc.what()));
	}

#ifdef _WIN32
	std::cout << "Press enter to quit." << std::endl;
	std::string c;
	std::cin >> c;
#endif

	return 0;
}