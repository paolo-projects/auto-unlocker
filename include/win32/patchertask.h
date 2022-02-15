#ifndef PATCHERTHREAD_H
#define PATCHERTHREAD_H

#include <Windows.h>
#include <string>
#include <functional>
#include "win32/task.h"
#include "win32/patchresult.h"
#include "logging/streamlogstrategy.h"
#include "logging/statusbarlogstrategy.h"
#include "logging/combinedlogstrategy.h"
#include "unlocker_win.h"

class MainWindow;

class PatcherTask : public Task<void, float, PatchResult>
{
public:
	PatcherTask(MainWindow& mainWindow);
	void setOnCompleteCallback(std::function<void(PatchResult)> completeCallback);
	void setOnProgressCallback(std::function<void(float)> progressCallback);
protected:
	void onProgressUpdate(float progress) override;
	PatchResult doInBackground(void* arg) override;
	void onPostExecute(PatchResult result) override;
private:
	MainWindow& mainWindow;

	std::function<void(PatchResult)> onCompleteCallback = nullptr;
	std::function<void(float)> onProgressCallback = nullptr;

	void downloadProgress(float progress);
};

#endif