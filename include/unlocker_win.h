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

#include <stdio.h>

#define CHECKRES(x) try{ (x); } catch (const PatchException& exc) { Logger::error(exc.what()); }
#define KILL(x) (x); exit(1);

// Forward declarations

bool downloadTools(fs::path path, std::function<void(float)> progressCallback = nullptr);
void copyTools(fs::path toolspath, fs::path copyTo);

// Windows-only GUI-compatible functions
void preparePatchWin(fs::path backupPath);
void applyPatchWin(const fs::path& vmwareInstallPath, const fs::path& vmwareInstallPath64);
void uninstallWin(const fs::path& vmwareInstallPath, const fs::path& vmwareInstallPath64);
void stopServices();
void restartServices();

#endif // UNLOCKERWIN_H