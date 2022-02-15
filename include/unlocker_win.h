#ifndef UNLOCKERWIN_H
#define UNLOCKERWIN_H

#include <string>
#include "filesystem.hpp"
#include <map>
#include <stdexcept>

#include <curl/curl.h>

#include "config.h"
#include "network.h"
#include "versionparser.h"
#include "debug.h"
#include "toolsdownloader.h"
#include "installinfo.h"
#include "winservices.h"
#include "patcher.h"
#include "patchversioner.h"

#include <stdio.h>

bool downloadTools(fs::path path, std::function<void(double, double, double, double)> progressCallback = nullptr);
void copyTools(fs::path toolspath, fs::path copyTo);

void preparePatchWin(fs::path backupPath, fs::path vmInstallPath);
void applyPatchWin(const fs::path& vmwareInstallPath, const fs::path& vmwareInstallPath64);
void stopServices();
void restartServices();

#endif // UNLOCKERWIN_H