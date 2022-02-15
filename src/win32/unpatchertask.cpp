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
		uninstallWin(vmwareInstallDir, vmwareInstallDir64);
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
