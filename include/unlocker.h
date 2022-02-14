#ifndef UNLOCKER_H
#define UNLOCKER_H

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

#ifdef __linux__
#include <unistd.h>
#include <strings.h>

#define stricmp(a, b) strcasecmp(a, b)
#endif

#include <stdio.h>

#define CHECKRES(x) try{ (x); } catch (const PatchException& exc) { Logger::error(exc.what()); }
#define KILL(x) (x); exit(1);

// Forward declarations

void preparePatch(fs::path backupPath);
bool downloadTools(fs::path path, std::function<void(float)> progressCallback = nullptr);
void copyTools(fs::path toolspath);
void stopServices();
void restartServices();

void install();

#ifdef _WIN32
void doPatch(const fs::path& vmwareInstallPath, const fs::path& vmwareInstallPath64);
void uninstall(const fs::path& vmwareInstallPath, const fs::path& vmwareInstallPath64);
#else
void doPatch();
void uninstall();
#endif

#endif // UNLOKER_H