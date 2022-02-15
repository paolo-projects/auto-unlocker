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

void PatcherTask::downloadProgress(double dltotal, double dlnow, double ultotal, double ulnow)
{
	static char statusBarProgress[1024];

	using namespace std::chrono;
	milliseconds tTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	long long tDelta = (tTime - lastProgressUpdate).count();

	if (dltotal > 0 && tDelta > PROG_PERIOD_MS) {
		double dlDelta = dlnow - lastDlNow;
		float downloadPercent = dlnow / dltotal;

		postProgress(0.2f + downloadPercent * 0.6f);

		sprintf(statusBarProgress, "Downloading: %.0f %%, %.3f MB/s", downloadPercent*100, (dlDelta*1000)/(tDelta*1024*1024));
		mainWindow.statusBar->setText(statusBarProgress);
		lastProgressUpdate = tTime;
		lastDlNow = dlnow;
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
		PatchVersioner patchVersion(vmwareInstallDir);

		if (patchVersion.hasPatch())
		{
			throw std::runtime_error("The vmware installation you specified is already patched. Uninstall the previous patch first, or delete the .unlocker file (not suggested)");
		}

		Logger::info("Killing services and backing up files...");
		preparePatchWin(backup, vmwareInstallDir);

		postProgress(0.1f);

		Logger::info("Patching files...");
		applyPatchWin(vmwareInstallDir, vmwareInstallDir64);

		patchVersion.writePatchData();

		postProgress(0.2f);

		// Default output path is ./tools/
		fs::path toolsdirectory = fs::path(".") / TOOLS_DOWNLOAD_FOLDER;

		lastProgressUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
			);
		lastDlNow = 0.0;

		if (mainWindow.downloadToolsChk->isChecked())
		{
			Logger::info("Downloading tools into \"" + toolsdirectory.string() + "\" directory...");
			downloadTools(toolsdirectory, std::bind(&PatcherTask::downloadProgress, this, std::placeholders::_1, 
				std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
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
		Logger::error(exc.what());
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
