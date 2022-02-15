#include "win32\downloadtoolstask.h"
#include "win32\mainwindow.h"

DownloadToolsTask::DownloadToolsTask(MainWindow& mainWindow)
	: mainWindow(mainWindow)
{
}

void DownloadToolsTask::setOnCompleteCallback(std::function<void(PatchResult)> completeCallback)
{
	onCompleteCallback = completeCallback;
}

void DownloadToolsTask::setOnProgressCallback(std::function<void(float)> progressCallback)
{
	onProgressCallback = progressCallback;
}

void DownloadToolsTask::onProgressUpdate(float progress)
{
	if (onProgressCallback != nullptr) {
		onProgressCallback(progress);
	}
}

void DownloadToolsTask::downloadProgress(double dltotal, double dlnow, double ultotal, double ulnow)
{
	static char statusBarProgress[1024];

	using namespace std::chrono;
	milliseconds tTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	long long tDelta = (tTime - lastProgressUpdate).count();

	if (dltotal > 0 && tDelta > PROG_PERIOD_MS) {
		double dlDelta = dlnow - lastDlNow;
		float downloadPercent = dlnow / dltotal;

		postProgress(downloadPercent);

		sprintf(statusBarProgress, "Downloading: %.0f %%, %.3f MB/s", downloadPercent * 100, (dlDelta * 1000) / (tDelta * 1024 * 1024));
		mainWindow.statusBar->setText(statusBarProgress);
		lastProgressUpdate = tTime;
		lastDlNow = dlnow;
	}
}

PatchResult DownloadToolsTask::doInBackground(void* arg)
{
	postProgress(0.f);

	// Create a log file in the current directory named unlocker-download-{date}.log
	char logFilePath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, logFilePath);

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << "unlocker-download-" << std::put_time(&tm, "%d-%m-%Y.%H-%M-%S") << ".log";
	auto str = oss.str();

	PathAppend(logFilePath, str.c_str());

	std::ofstream logStream(logFilePath);
	StreamLogStrategy logStrategy(logStream);
	StatusBarLogStrategy statusLogStrategy(mainWindow.statusBar.get());

	CombinedLogStrategy multipleLogStrategy;
	multipleLogStrategy.add(&logStrategy);
	multipleLogStrategy.add(&statusLogStrategy);

	Logger::init(&multipleLogStrategy);

	try {
		postProgress(0.f);

		lastProgressUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
			);
		lastDlNow = 0.0;

		fs::path toolsdirectory = fs::path(mainWindow.toolsPathEditBox->getText());
		downloadTools(toolsdirectory, std::bind(&DownloadToolsTask::downloadProgress, this, std::placeholders::_1,
			std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

		postProgress(1.f);

		Logger::info("Tools downloaded.");
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

void DownloadToolsTask::onPostExecute(PatchResult result)
{
	if (onCompleteCallback != nullptr)
	{
		onCompleteCallback(result);
	}
}
