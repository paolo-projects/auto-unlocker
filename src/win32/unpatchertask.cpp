#include "win32\unpatchertask.h"
#include "win32\mainwindow.h"

UnpatcherTask::UnpatcherTask(MainWindow& mainWindow)
	:	mainWindow(mainWindow)
{
}

void UnpatcherTask::setOnCompleteCallback(std::function<void(PatchResult)> completeCallback)
{
	onCompleteCallback = completeCallback;
}

void UnpatcherTask::setOnProgressCallback(std::function<void(float)> progressCallback)
{
	onProgressCallback = progressCallback;
}

void UnpatcherTask::onProgressUpdate(float progress)
{
	if (onProgressCallback != nullptr)
	{
		onProgressCallback(progress);
	}
}

PatchResult UnpatcherTask::doInBackground(void* arg)
{
	fs::path vmwareInstallDir = fs::path(mainWindow.pathEditBox->getText()),
		vmwareInstallDir64 = fs::path(mainWindow.pathEditBoxX64->getText());

	// Create a log file in the current directory named unlocker-uninstall-{date}.log
	char logFilePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, logFilePath);

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << "unlocker-uninstall-" << std::put_time(&tm, "%d-%m-%Y.%H-%M-%S") << ".log";
	auto str = oss.str();

	PathAppend(logFilePath, str.c_str());

	std::ofstream logStream(logFilePath);
	StreamLogStrategy logStrategy(logStream);
	StatusBarLogStrategy statusLogStrategy(mainWindow.statusBar.get());

	CombinedLogStrategy multipleLogStrategy;
	multipleLogStrategy.add(&logStrategy);
	multipleLogStrategy.add(&statusLogStrategy);

	Logger::init(&multipleLogStrategy);
	try
	{
		postProgress(0.f); PatchVersioner patchVersion(vmwareInstallDir);

		if (!patchVersion.hasPatch())
		{
			throw std::runtime_error("The vmware installation you specified does not have a patch installed");
		}

		// Stop services
		stopServices();

		postProgress(0.2f);

		// Default output path is ./tools/
		fs::path toolsdirectory = fs::path(".") / TOOLS_DOWNLOAD_FOLDER;
		// Default backup path is ./backup/
		fs::path backup = fs::path(".") / BACKUP_FOLDER;

		Logger::info("Restoring files...");
		// Copy contents of backup/
		if (fs::exists(backup))
		{
			for (const auto& file : fs::directory_iterator(backup))
			{
				if (fs::is_regular_file(file))
				{
					if (fs::copy_file(file.path(), vmwareInstallDir / file.path().filename(), fs::copy_options::overwrite_existing))
					{
						Logger::info("File \"" + file.path().string() + "\" restored successfully");
					}
					else
					{
						throw std::runtime_error("Couldn't restore file " + file.path().string());
					}
				}
			}
			// Copy contents of backup/x64/
			for (const auto& file : fs::directory_iterator(backup / "x64"))
			{
				if (fs::is_regular_file(file))
				{
					if (fs::copy_file(file.path(), vmwareInstallDir64 / file.path().filename(), fs::copy_options::overwrite_existing))
					{
						Logger::info("File \"" + file.path().string() + "\" restored successfully");
					}
					else
					{
						throw std::runtime_error("Couldn't restore file " + file.path().string());
					}
				}
			}
		}
		else {
			throw std::runtime_error("Couldn't find backup files...");
		}

		postProgress(0.4f);

		// Remove darwin*.* from InstallDir
		for (const auto& file : fs::directory_iterator(vmwareInstallDir))
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

		fs::remove_all(backup);
		fs::remove_all(toolsdirectory);

		postProgress(0.6f);

		// Restart services
		restartServices();

		postProgress(0.8f);

		patchVersion.removePatchVersion();

		Logger::info("Uninstall complete.");

		postProgress(1.f);
	}
	catch (const std::runtime_error& exc)
	{
		Logger::free();
		return PatchResult{
			false,
			std::string(exc.what()),
			std::string(logFilePath)
		};
	}

	Logger::free();
	return PatchResult{
		true,
		"",
		std::string(logFilePath)
	};
}

void UnpatcherTask::onPostExecute(PatchResult result)
{
	if (onCompleteCallback != nullptr)
	{
		onCompleteCallback(result);
	}
}
