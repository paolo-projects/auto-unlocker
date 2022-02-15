#include "win32/mainwindow.h"

void MainWindow::onCreate(HWND hWnd)
{
	VMWareInfoRetriever vmwareInfo;

	char backupPath[MAX_PATH], toolsPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, backupPath);
	strcpy(toolsPath, backupPath);
	PathAppend(toolsPath, "tools");
	std::string toolsDefLoc(toolsPath);

	PathAppend(backupPath, "backup");
	DWORD dwBackupFolderAttrs = GetFileAttributes(backupPath);
	bool backupExists = (dwBackupFolderAttrs != INVALID_FILE_ATTRIBUTES) && (dwBackupFolderAttrs & FILE_ATTRIBUTE_DIRECTORY);

	// Add controls to the window
	browseLabel = std::make_unique<Label>(hWnd, NULL, "VMWare install location", 10, 10, 430, 15);
	pathEditBox = std::make_unique<EditBox>(hWnd, IDC_PATH_EDITBOX, vmwareInfo.getInstallPath().c_str(), 10, 35, 300, 30);
	browseButton = std::make_unique<Button>(hWnd, IDC_PATH_BROWSEBTN, "Browse", 320, 35, 100, 30);
	browseLabelX64 = std::make_unique<Label>(hWnd, NULL, "VMWare X64 location", 10, 75, 430, 15);
	pathEditBoxX64 = std::make_unique<EditBox>(hWnd, IDC_PATH_EDITBOXX64, vmwareInfo.getInstallPath64().c_str(), 10, 100, 300, 30);
	browseButtonX64 = std::make_unique<Button>(hWnd, IDC_PATH_BROWSEBTNX64, "Browse", 320, 100, 100, 30);
	toolsGroup = std::make_unique<GroupBox>(hWnd, "Tools", 10, 140, 410, 135);
	downloadToolsChk = std::make_unique<CheckBox>(hWnd, IDC_DOWNLOADTOOLS_CHECKBOX, "Download tools", 20, 160, 400, 30);
	toolsPathLabel = std::make_unique<Label>(hWnd, NULL, "Existing tools location:", 20, 200, 400, 15);
	toolsPathEditBox = std::make_unique<EditBox>(hWnd, IDC_TOOLSPATH_EDITBOX, toolsDefLoc.c_str(), 20, 225, 255, 30);
	toolsBrowseBtn = std::make_unique<Button>(hWnd, IDC_TOOLSPATH_BROWSE_BTN, "Browse", 280, 225, 60, 30);
	downloadToolsBtn = std::make_unique<Button>(hWnd, IDC_TOOLS_DOWNLOAD_BTN, "Download", 340, 225, 70, 30);
	patchBtn = std::make_unique<Button>(hWnd, IDC_PATCH_BTN, "Patch", 10, 285, 100, 30);
	revertPatchBtn = std::make_unique<Button>(hWnd, IDC_REVERT_PATCH_BTN, "Uninstall patch", 115, 285, 100, 30);
	progressBar = std::make_unique<Progress>(hWnd, IDC_PROGRESSBAR, 10, 325, 410, 25);
	statusBar = std::make_unique<StatusBar>(hWnd, IDC_STATUSBAR, "Auto-Unlocker " PROG_VERSION);

	// Initial controls setup
	pathEditBox->setReadOnly(true);
	pathEditBoxX64->setReadOnly(true);
	toolsPathEditBox->setReadOnly(true);
	downloadToolsChk->set(true);
	toolsPathEditBox->setEnabled(false);
	revertPatchBtn->setEnabled(backupExists);
	progressBar->setRange(100);
	progressBar->setProgress(0);

	// Add listeners
	browseButton->addEventListener(EventType::CLICK_EVENT, std::bind(&MainWindow::browseButtonClick, this));
	browseButtonX64->addEventListener(EventType::CLICK_EVENT, std::bind(&MainWindow::browseX64ButtonClick, this));
	downloadToolsChk->addEventListener(EventType::CLICK_EVENT, std::bind(&MainWindow::downloadToolsChkClick, this));
	toolsBrowseBtn->addEventListener(EventType::CLICK_EVENT, std::bind(&MainWindow::toolsBrowseButtonClick, this));
	patchBtn->addEventListener(EventType::CLICK_EVENT, std::bind(&MainWindow::patchBtnClick, this));
	revertPatchBtn->addEventListener(EventType::CLICK_EVENT, std::bind(&MainWindow::revertPatchBtnClick, this));
	downloadToolsBtn->addEventListener(EventType::CLICK_EVENT, std::bind(&MainWindow::downloadToolsBtnClick, this));

	// Register controls for events dispatch
	registerControl(IDC_BROWSELABEL, browseLabel.get());
	registerControl(IDC_PATH_EDITBOX, pathEditBox.get());
	registerControl(IDC_PATH_BROWSEBTN, browseButton.get());
	registerControl(IDC_BROWSELABEL_X64, browseLabelX64.get());
	registerControl(IDC_PATH_EDITBOXX64, pathEditBoxX64.get());
	registerControl(IDC_PATH_BROWSEBTNX64, browseButtonX64.get());
	registerControl(IDC_TOOLS_GROUPBOX, toolsGroup.get());
	registerControl(IDC_DOWNLOADTOOLS_CHECKBOX, downloadToolsChk.get());
	registerControl(IDC_TOOLSPATH_EDITBOX, toolsPathEditBox.get());
	registerControl(IDC_TOOLSPATH_BROWSE_BTN, toolsBrowseBtn.get());
	registerControl(IDC_PATCH_BTN, patchBtn.get());
	registerControl(IDC_REVERT_PATCH_BTN, revertPatchBtn.get());
	registerControl(IDC_TOOLS_DOWNLOAD_BTN, downloadToolsBtn.get());
}

void MainWindow::browseButtonClick()
{
	char path[MAX_PATH];
	BROWSEINFO bInfo = {};
	bInfo.lpszTitle = "Select the VMWare install directory";
	bInfo.hwndOwner = hWnd;
	LPITEMIDLIST pIdl = SHBrowseForFolder(&bInfo);

	if (pIdl != 0) {
		SHGetPathFromIDList(pIdl, path);
		pathEditBox->setText(std::string(path));
	}
}

void MainWindow::browseX64ButtonClick()
{
	char path[MAX_PATH];
	BROWSEINFO bInfo = {};
	bInfo.lpszTitle = "Select the VMWare x64 install directory";
	bInfo.hwndOwner = hWnd;
	LPITEMIDLIST pIdl = SHBrowseForFolder(&bInfo);

	if (pIdl != 0) {
		SHGetPathFromIDList(pIdl, path);
		pathEditBoxX64->setText(std::string(path));
	}
}

void MainWindow::toolsBrowseButtonClick()
{
	char path[MAX_PATH];
	BROWSEINFO bInfo = {};
	bInfo.lpszTitle = "Select the VMWare x64 install directory";
	bInfo.hwndOwner = hWnd;
	LPITEMIDLIST pIdl = SHBrowseForFolder(&bInfo);

	if (pIdl != 0) {
		SHGetPathFromIDList(pIdl, path);
		toolsPathEditBox->setText(std::string(path));
	}
}

void MainWindow::downloadToolsChkClick()
{
	bool isChecked = downloadToolsChk->isChecked();

	toolsPathEditBox->setEnabled(!isChecked);
}

void MainWindow::patchBtnClick()
{
	disableAllInput();

	patcherTask = new PatcherTask(*this);

	patcherTask->setOnProgressCallback(std::bind(&MainWindow::patchProgress, this, std::placeholders::_1));
	patcherTask->setOnCompleteCallback(std::bind(&MainWindow::patchComplete, this, std::placeholders::_1));

	patcherTask->run(nullptr);
}

void MainWindow::revertPatchBtnClick()
{
	disableAllInput();

	unpatcherTask = new UnpatcherTask(*this);

	unpatcherTask->setOnProgressCallback(std::bind(&MainWindow::patchProgress, this, std::placeholders::_1));
	unpatcherTask->setOnCompleteCallback(std::bind(&MainWindow::unpatchComplete, this, std::placeholders::_1));

	unpatcherTask->run(nullptr);
}

void MainWindow::downloadToolsBtnClick()
{
	disableAllInput();
	
	downloadToolsTask = new DownloadToolsTask(*this);

	downloadToolsTask->setOnProgressCallback(std::bind(&MainWindow::patchProgress, this, std::placeholders::_1));
	downloadToolsTask->setOnCompleteCallback(std::bind(&MainWindow::toolsDownloadComplete, this, std::placeholders::_1));

	downloadToolsTask->run(nullptr);
}

void MainWindow::disableAllInput()
{
	dlgState.pathBrowseEnabled = browseButton->isEnabled();
	browseButton->setEnabled(false);
	dlgState.pathEnabled = pathEditBox->isEnabled();
	pathEditBox->setEnabled(false);
	dlgState.path64BrowseEnabled = browseButtonX64->isEnabled();
	browseButtonX64->setEnabled(false);
	dlgState.path64Enabled = pathEditBoxX64->isEnabled();
	pathEditBoxX64->setEnabled(false);
	dlgState.downloadToolsChecked = downloadToolsChk->isChecked();
	dlgState.downloadToolsEnabled = downloadToolsChk->isEnabled();
	downloadToolsChk->setEnabled(false);
	dlgState.toolsPathEnabled = toolsPathEditBox->isEnabled();
	toolsPathEditBox->setEnabled(false);
	dlgState.toolsBrowseEnabled = toolsBrowseBtn->isEnabled();
	toolsBrowseBtn->setEnabled(false);
	dlgState.downloadToolsBtnEnabled = downloadToolsBtn->isEnabled();
	downloadToolsBtn->setEnabled(false);
	dlgState.patchEnabled = patchBtn->isEnabled();
	patchBtn->setEnabled(false);
	dlgState.unpatchEnabled = revertPatchBtn->isEnabled();
	revertPatchBtn->setEnabled(false);
}

void MainWindow::restoreInput()
{
	browseButton->setEnabled(dlgState.pathBrowseEnabled);
	pathEditBox->setEnabled(dlgState.pathEnabled);
	browseButtonX64->setEnabled(dlgState.path64BrowseEnabled);
	pathEditBoxX64->setEnabled(dlgState.path64Enabled);
	downloadToolsChk->set(dlgState.downloadToolsChecked);
	downloadToolsChk->setEnabled(dlgState.downloadToolsEnabled);
	toolsPathEditBox->setEnabled(dlgState.toolsPathEnabled);
	toolsBrowseBtn->setEnabled(dlgState.toolsBrowseEnabled);
	downloadToolsBtn->setEnabled(dlgState.downloadToolsBtnEnabled);
	patchBtn->setEnabled(dlgState.patchEnabled);
	revertPatchBtn->setEnabled(dlgState.unpatchEnabled);

	progressBar->setProgress(0);
}

void MainWindow::patchComplete(PatchResult result)
{
	if (result.result) {
		char msg[4096];
		sprintf(msg, "The patch has been installed successfully. Refer to the log file for detailed info:\n%s", result.logFilePath.c_str());
		MessageBox(hWnd, msg,
			"Success", MB_OK | MB_ICONINFORMATION);
	}
	else {
		char msg[4096];
		sprintf(msg, "An error occurred while applying the patch:\n%s\nCheck the log file for detailed info:\n%s", result.errorMessage.c_str(), result.logFilePath.c_str());
		MessageBox(hWnd, msg, "Error", MB_OK | MB_ICONERROR);
	}

	if (patcherTask != nullptr)
	{
		delete patcherTask;
		patcherTask = nullptr;
	}

	restoreInput();
}

void MainWindow::unpatchComplete(PatchResult result)
{
	if (result.result) {
		char msg[4096];
		sprintf(msg, "The patch has been uninstalled successfully. Refer to the log file for detailed info:\n%s", result.logFilePath.c_str());
		MessageBox(hWnd, msg,
			"Success", MB_OK | MB_ICONINFORMATION);
	}
	else {
		char msg[4096];
		sprintf(msg, "An error occurred while uninstalling the patch:\n%s\nCheck the log file for detailed info:\n%s", result.errorMessage.c_str(), result.logFilePath.c_str());
		MessageBox(hWnd, msg, "Error", MB_OK | MB_ICONERROR);
	}

	if (unpatcherTask != nullptr)
	{
		delete unpatcherTask;
		unpatcherTask = nullptr;
	}

	restoreInput();
}

void MainWindow::toolsDownloadComplete(PatchResult result)
{
	if (result.result) {
		char msg[4096];
		sprintf(msg, "The tools have been downloaded successfully. Refer to the log file for detailed info:\n%s", result.logFilePath.c_str());
		MessageBox(hWnd, msg,
			"Success", MB_OK | MB_ICONINFORMATION);
	}
	else {
		char msg[4096];
		sprintf(msg, "An error occurred while downloading the tools:\n%s\nCheck the log file for detailed info:\n%s", result.errorMessage.c_str(), result.logFilePath.c_str());
		MessageBox(hWnd, msg, "Error", MB_OK | MB_ICONERROR);
	}

	if (downloadToolsTask != nullptr)
	{
		delete downloadToolsTask;
		downloadToolsTask = nullptr;
	}

	restoreInput();
}

void MainWindow::patchProgress(float progress)
{
	progressBar->setProgress((int)(progress * 100));
}

MainWindow::MainWindow(HINSTANCE hInstance, int nCmdShow)
	: Window(hInstance, nCmdShow, "auto-unlocker-mainwindow", "Auto-Unlocker " PROG_VERSION, CW_USEDEFAULT, CW_USEDEFAULT, 450, 420)
{
}

MainWindow::~MainWindow()
{
}
