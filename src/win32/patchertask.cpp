#include "win32\patchertask.h"
#include "win32\mainwindow.h"

PatcherTask::PatcherTask(MainWindow& mainWindow)
	: mainWindow(mainWindow)
{
}

void PatcherTask::setOnCompleteCallback(std::function<void(PatchResult)> completeCallback)
{
	onCompleteCallback = completeCallback;
}

void PatcherTask::setOnProgressCallback(std::function<void(float)> progressCallback)
{
	onProgressCallback = progressCallback;
}

void PatcherTask::onProgressUpdate(float progress)
{
	if (onProgressCallback != nullptr) {
		onProgressCallback(progress);
	}
}

void PatcherTask::downloadProgress(float progress)
{
	if (progress > 0.f && progress < 1.f) {
		postProgress(0.2 + progress * 0.6);
	}
}

PatchResult PatcherTask::doInBackground(void* arg)
{
	postProgress(0.f);

	// If tools download is disabled check that the tools exist in the path provided through the GUI
	if (!mainWindow.downloadToolsChk->isChecked() &&
		!(fs::exists(fs::path(".") / TOOLS_DOWNLOAD_FOLDER / FUSION_ZIP_TOOLS_NAME) && fs::exists(fs::path(".") / TOOLS_DOWNLOAD_FOLDER / FUSION_ZIP_PRE15_TOOLS_NAME))
		)
	{
		return PatchResult{ false, "Tools not found in the directory you provided. Check the folder path or enable the option to download them" };
	}

	// Create a log file in the current directory named unlocker-{date}.log
	char logFilePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, logFilePath);

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << "unlocker-" << std::put_time(&tm, "%d-%m-%Y.%H-%M-%S") << ".log";
	auto str = oss.str();

	PathAppend(logFilePath, str.c_str());

	std::ofstream logStream(logFilePath);
	StreamLogStrategy logStrategy(logStream);
	StatusBarLogStrategy statusLogStrategy(mainWindow.statusBar.get());

	CombinedLogStrategy multipleLogStrategy;
	multipleLogStrategy.add(&logStrategy);
	multipleLogStrategy.add(&statusLogStrategy);

	Logger::init(&multipleLogStrategy);

	// Default backup path is ./backup/
	fs::path backup = fs::path(".") / BACKUP_FOLDER;

	fs::path vmwareInstallDir = fs::path(mainWindow.pathEditBox->getText()),
		vmwareInstallDir64 = fs::path(mainWindow.pathEditBoxX64->getText());

	try {
		Logger::info("Killing services and backing up files...");
		preparePatchWin(backup);

		postProgress(0.1f);

		Logger::info("Patching files...");
		applyPatchWin(vmwareInstallDir, vmwareInstallDir64);

		postProgress(0.2f);

		// Default output path is ./tools/
		fs::path toolsdirectory = fs::path(".") / TOOLS_DOWNLOAD_FOLDER;

		if (mainWindow.downloadToolsChk->isChecked())
		{
			Logger::info("Downloading tools into \"" + toolsdirectory.string() + "\" directory...");
			downloadTools(toolsdirectory, std::bind(&PatcherTask::downloadProgress, this, std::placeholders::_1));
		}
		else {
			toolsdirectory = fs::path(mainWindow.toolsPathEditBox->getText());
		}

		postProgress(0.8f);

		Logger::info("Copying tools into program directory...");
		copyTools(toolsdirectory, vmwareInstallDir);

		postProgress(0.9f);

		restartServices();

		postProgress(1.f);

		Logger::info("Patch complete.");
	}
	catch (const std::runtime_error& exc)
	{
		Logger::free();
		return PatchResult{ false, std::string(exc.what()), std::string(logFilePath) };
	}

	Logger::free();
	return PatchResult{ true, "", std::string(logFilePath) };
}

void PatcherTask::onPostExecute(PatchResult result)
{
	if (onCompleteCallback != nullptr)
	{
		onCompleteCallback(result);
	}
}
