#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include "config.h"
#include <iostream>
#include <stdarg.h>
#include "colors.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#define LOGLEVEL_VERBOSE 3
#define LOGLEVEL_DEBUG 2
#define LOGLEVEL_INFO 1
#define LOGLEVEL_NONE 0

void logv(std::string msg);
void logv(const char* msg, ...);

void logd(std::string msg);
void logd(const char* msg, ...);

void logi(std::string msg);
void logi(const char* msg, ...);

void logerr(std::string err);
void logerr(const char* err, ...);

#ifdef _WIN32
void setTerminalColor(WORD tAttribute, bool isStderr = false);
void resetTerminalColor(bool isStderr = false);
#endif

#endif // DEBUGUTILS_H
