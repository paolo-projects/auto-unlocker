#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include "config.h"
#include <iostream>
#include <stdarg.h>
#include "colors.h"
#include "logging/logstrategy.h"

#ifdef _WIN32
#include <Windows.h>
#include "win32/controls/statusbar.h"
#endif

#define LOGLEVEL_VERBOSE 3
#define LOGLEVEL_DEBUG 2
#define LOGLEVEL_INFO 1
#define LOGLEVEL_NONE 0

class Logger
{
public:
	static void init(LogStrategy* strategy);
	static void free();

	static void verbose(std::string msg);
	static void verbose(const char* msg, ...);

	static void debug(std::string msg);
	static void debug(const char* msg, ...);

	static void info(std::string msg);
	static void info(const char* msg, ...);

	static void error(std::string err);
	static void error(const char* err, ...);

#ifdef _WIN32
	static void printWinDebug(const char* fmt, ...);
#endif
private:
	Logger(LogStrategy* strategy);

	static Logger* instance;
	LogStrategy* logStrategy;
};

#endif // DEBUGUTILS_H
