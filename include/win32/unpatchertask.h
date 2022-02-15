#ifndef UNPATCHERTASK_H
#define UNPATCHERTASK_H

#include <functional>
#include "win32/task.h"
#include "win32/patchresult.h"
#include "unlocker_win.h"

class MainWindow;

class UnpatcherTask : public Task<void, float, PatchResult>
{
public:
	UnpatcherTask(MainWindow& mainWindow);
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
};

#endif