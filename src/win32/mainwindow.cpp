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
	toolsPathLabel = std::make_unique<Label>(hWnd, NULL, "Tools location:", 20, 200, 400, 15);
	toolsPathEditBox = std::make_unique<EditBox>(hWnd, IDC_TOOLSPATH_EDITBOX, toolsDefLoc.c_str(), 20, 235, 300, 30);
	toolsBrowseBtn = std::make_unique<Button>(hWnd, IDC_TOOLSPATH_BROWSE_BTN, "Browse", 340, 225, 70, 30);
	patchBtn = std::make_unique<Button>(hWnd, IDC_PATCH_BTN, "Patch", 10, 285, 100, 30);
	revertPatchBtn = std::make_unique<Button>(hWnd, IDC_REVERT_PATCH_BTN, "Revert patch", 115, 285, 100, 30);
	progressBar = std::make_unique<Progress>(hWnd, IDC_PROGRESSBAR, 10, 325, 410, 25);

	// Initial controls setup
	pathEditBox->setReadOnly(true);
	pathEditBoxX64->setReadOnly(true);
	toolsPathEditBox->setReadOnly(true);
	downloadToolsChk->set(true);
	toolsPathEditBox->setEnabled(false);
	toolsBrowseBtn->setEnabled(false);
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
	toolsBrowseBtn->setEnabled(!isChecked);
}

void MainWindow::patchBtnClick()
{

}

void MainWindow::revertPatchBtnClick()
{
}

MainWindow::MainWindow(HINSTANCE hInstance, int nCmdShow)
	: Window(hInstance, nCmdShow, "auto-unlocker-mainwindow", "Auto-Unlocker " PROG_VERSION, CW_USEDEFAULT, CW_USEDEFAULT, 450, 400)
{
}

MainWindow::~MainWindow()
{
}
