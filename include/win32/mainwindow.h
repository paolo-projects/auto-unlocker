#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <stdexcept>
#include <memory>
#include <functional>
#include "filesystem.hpp"

#include "config.h"
#include "controls/button.h";
#include "controls/editbox.h"
#include "controls/label.h"
#include "controls/window.h"
#include "controls/progress.h"
#include "controls/groupbox.h"
#include "controls/checkbox.h"
#include "controls/statusbar.h"
#include "patchertask.h"
#include "unpatchertask.h"
#include "downloadtoolstask.h"
#include "resources.h"

#include "installinfo.h"
#include "unlocker_win.h"

struct DialogState
{
	bool pathEnabled,
		path64Enabled,
		pathBrowseEnabled,
		path64BrowseEnabled,
		downloadToolsEnabled,
		downloadToolsChecked,
		toolsPathEnabled,
		toolsBrowseEnabled,
		downloadToolsBtnEnabled,
		patchEnabled,
		unpatchEnabled;
};

class MainWindow : public Window
{
public:
	MainWindow(HINSTANCE hInstance, int nCmdShow);
	~MainWindow();
	virtual void onCreate(HWND hWnd) override;

	std::unique_ptr<Label> browseLabel = nullptr;
	std::unique_ptr<Button> browseButton = nullptr;
	std::unique_ptr<EditBox> pathEditBox = nullptr;
	std::unique_ptr<Label> browseLabelX64 = nullptr;
	std::unique_ptr<Button> browseButtonX64 = nullptr;
	std::unique_ptr<EditBox> pathEditBoxX64 = nullptr;
	std::unique_ptr<Progress> progressBar = nullptr;
	std::unique_ptr<GroupBox> toolsGroup = nullptr;
	std::unique_ptr<CheckBox> downloadToolsChk = nullptr;
	std::unique_ptr<Label> toolsPathLabel = nullptr;
	std::unique_ptr<EditBox> toolsPathEditBox = nullptr;
	std::unique_ptr<Button> toolsBrowseBtn = nullptr;
	std::unique_ptr<Button> patchBtn = nullptr;
	std::unique_ptr<Button> revertPatchBtn = nullptr;
	std::unique_ptr<StatusBar> statusBar = nullptr;
	std::unique_ptr<Button> downloadToolsBtn = nullptr;

private:
	void browseButtonClick();
	void browseX64ButtonClick();
	void toolsBrowseButtonClick();
	void downloadToolsChkClick();
	void patchBtnClick();
	void revertPatchBtnClick();
	void downloadToolsBtnClick();

	void disableAllInput();
	void restoreInput();
	void patchComplete(PatchResult result);
	void unpatchComplete(PatchResult result);
	void toolsDownloadComplete(PatchResult result);
	void patchProgress(float progress);

	PatcherTask* patcherTask = nullptr;
	UnpatcherTask* unpatcherTask = nullptr;
	DownloadToolsTask* downloadToolsTask = nullptr;

	DialogState dlgState;
};

#endif // MAINWINDOW_H