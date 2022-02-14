#include "win32\patcherthread.h"
#include "win32\mainwindow.h"

PatcherThread::PatcherThread(MainWindow& mainWindow)
	: mainWindow(mainWindow)
{
}

void PatcherThread::execute()
{
	DWORD dwThreadId;

	CreateThread(
		NULL,
		0,
		PatcherThread::threadFunction,
		this,
		0,
		&dwThreadId);
}

void PatcherThread::setOnCompleteCallback(std::function<void(PatchResult)> completeCallback)
{
	onCompleteCallback = completeCallback;
}

void PatcherThread::setOnProgressCallback(std::function<void(float)> progressCallback)
{
	onProgressCallback = progressCallback;
}

void PatcherThread::publishProgress(float progress)
{
	if (onProgressCallback != nullptr) {
		onProgressCallback(progress);
	}
}

void PatcherThread::downloadProgress(float progress)
{
	if (progress > 0.f && progress < 1.f) {
		publishProgress(0.2 + progress * 0.6);
	}
}

DWORD PatcherThread::threadFunction(void* lpThreadParameter)
{
	PatcherThread* instance = reinterpret_cast<PatcherThread*>(lpThreadParameter);

	instance->publishProgress(0.f);

	// If tools download is disabled check that the tools exist in the path provided through the GUI
	if (!instance->mainWindow.downloadToolsChk->isChecked() &&
		!(fs::exists(fs::path(".") / TOOLS_DOWNLOAD_FOLDER / FUSION_ZIP_TOOLS_NAME) && fs::exists(fs::path(".") / TOOLS_DOWNLOAD_FOLDER / FUSION_ZIP_PRE15_TOOLS_NAME))
		)
	{
		if (instance->onCompleteCallback != nullptr) {
			instance->onCompleteCallback(PatchResult{ false, "Tools not found in the directory you provided. Check the folder path or enable the option to download them" });
		}
		return 1;
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
	StatusBarLogStrategy statusLogStrategy(instance->mainWindow.statusBar.get());

	CombinedLogStrategy multipleLogStrategy;
	multipleLogStrategy.add(&logStrategy);
	multipleLogStrategy.add(&statusLogStrategy);

	Logger::init(&multipleLogStrategy);

	// Default backup path is ./backup/
	fs::path backup = fs::path(".") / BACKUP_FOLDER;

	try {
		Logger::info("Killing services and backing up files...");
		preparePatch(backup);

		instance->publishProgress(0.1f);

		Logger::info("Patching files...");
		doPatch(fs::path(instance->mainWindow.pathEditBox->getText()), fs::path(instance->mainWindow.pathEditBoxX64->getText()));

		instance->publishProgress(0.2f);

		// Default output path is ./tools/
		fs::path toolsdirectory = fs::path(".") / TOOLS_DOWNLOAD_FOLDER;

		if (instance->mainWindow.downloadToolsChk->isChecked())
		{
			Logger::info("Downloading tools into \"" + toolsdirectory.string() + "\" directory...");
			downloadTools(toolsdirectory, std::bind(&PatcherThread::downloadProgress, instance, std::placeholders::_1));
		}
		else {
			toolsdirectory = fs::path(instance->mainWindow.toolsPathEditBox->getText());
		}

		instance->publishProgress(0.8f);

		Logger::info("Copying tools into program directory...");
		copyTools(toolsdirectory);

		instance->publishProgress(0.9f);

		restartServices();

		instance->publishProgress(1.f);

		Logger::info("Patch complete.");

		if (instance->onCompleteCallback != nullptr) {
			instance->onCompleteCallback(PatchResult{ true, "" });
		}
	}
	catch (const std::exception& exc)
	{
		if (instance->onCompleteCallback != nullptr) {
			instance->onCompleteCallback(PatchResult{ false, std::string(exc.what())});
		}
	}

	Logger::free();

	return 0;
}
