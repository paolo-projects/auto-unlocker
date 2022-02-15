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

void DownloadToolsTask::downloadProgress(float progress)
{
	if (progress > 0.f && progress < 1.f) {
		postProgress(progress * 0.8);
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

		fs::path toolsdirectory = fs::path(mainWindow.toolsPathEditBox->getText());
		downloadTools(toolsdirectory, std::bind(&DownloadToolsTask::downloadProgress, this, std::placeholders::_1));

		postProgress(1.f);

		Logger::info("Tools downloaded.");
	}
	catch (const std::runtime_error& exc)
	{
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
