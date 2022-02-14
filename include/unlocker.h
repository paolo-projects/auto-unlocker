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
void doPatch();
bool downloadTools(fs::path path);
void copyTools(fs::path toolspath);
void stopServices();
void restartServices();

void install();
void uninstall();
void showhelp();

#endif // UNLOKER_H