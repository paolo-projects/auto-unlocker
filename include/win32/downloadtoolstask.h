#ifndef DOWNLOADTOOLSTASK_H
#define DOWNLOADTOOLSTASK_H

#include <Windows.h>
#include <string>
#include <functional>
#include <chrono>

#include "win32/task.h"
#include "win32/patchresult.h"
#include "logging/streamlogstrategy.h"
#include "logging/statusbarlogstrategy.h"
#include "logging/combinedlogstrategy.h"
#include "unlocker_win.h"

class MainWindow;

class DownloadToolsTask : public Task<void, float, PatchResult>
{
public:
	DownloadToolsTask(MainWindow& mainWindow);
	void setOnCompleteCallback(std::function<void(PatchResult)> completeCallback);
	void setOnProgressCallback(std::function<void(float)> progressCallback);
protected:
	void onProgressUpdate(float progress) override;
	PatchResult doInBackground(void* arg) override;
	void onPostExecute(PatchResult result) override;
private:
	static constexpr int PROG_PERIOD_MS = 200;

	MainWindow& mainWindow;

	std::function<void(PatchResult)> onCompleteCallback = nullptr;
	std::function<void(float)> onProgressCallback = nullptr;

	void downloadProgress(double dltotal, double dlnow, double ultotal, double ulnow);

	std::chrono::milliseconds lastProgressUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		);
	double lastDlNow = 0.0;
};

#endif