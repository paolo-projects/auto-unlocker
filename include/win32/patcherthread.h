#ifndef PATCHERTHREAD_H
#define PATCHERTHREAD_H

#include <Windows.h>
#include <string>
#include <functional>
#include "logging/streamlogstrategy.h"
#include "logging/statusbarlogstrategy.h"
#include "logging/combinedlogstrategy.h"

class MainWindow;

struct PatchResult
{
	bool result;
	std::string errorMessage;
};

class PatcherThread
{
public:
	PatcherThread(MainWindow& mainWindow);
	void execute();
	void setOnCompleteCallback(std::function<void(PatchResult)> completeCallback);
	void setOnProgressCallback(std::function<void(float)> progressCallback);
private:
	MainWindow& mainWindow;

	void publishProgress(float progress);

	static DWORD threadFunction(void* lpThreadParameter);
	std::function<void(PatchResult)> onCompleteCallback = nullptr;
	std::function<void(float)> onProgressCallback = nullptr;

	void downloadProgress(float progress);
};

#endif