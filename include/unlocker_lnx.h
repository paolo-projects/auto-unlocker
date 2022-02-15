#ifndef UNLOCKERLNX_H
#define UNLOCKERLNX_H

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
#include "patcher.h"

#include <unistd.h>
#include <strings.h>

#define stricmp(a, b) strcasecmp(a, b)

#include <stdio.h>

#define CHECKRES(x) try{ (x); } catch (const PatchException& exc) { Logger::error(exc.what()); }
#define KILL(x) (x); exit(1);

bool downloadTools(fs::path path, std::function<void(float)> progressCallback = nullptr);
void copyTools(fs::path toolspath);

void installLnx();
void preparePatchLnx(fs::path backupPath);
void applyPatchLnx();
void uninstallLnx();

#endif // UNLOKERLNX_H