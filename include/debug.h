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

class LogStrategy
{
public:
	virtual void verbose(const char* message) = 0;
	virtual void debug(const char* message) = 0;
	virtual void info(const char* message) = 0;
	virtual void error(const char* message) = 0;
};

class TerminalLogStrategy : public LogStrategy
{
public:
	virtual void verbose(const char* message) override;
	virtual void debug(const char* message) override;
	virtual void info(const char* message) override;
	virtual void error(const char* message) override;
};

class StreamLogStrategy : public LogStrategy
{
public:
	StreamLogStrategy(std::iostream& stream);

	virtual void verbose(const char* message) override;
	virtual void debug(const char* message) override;
	virtual void info(const char* message) override;
	virtual void error(const char* message) override;
private:
	std::iostream& stream;
};

class Logger
{
public:
	static void init(LogStrategy* strategy);

	static void verbose(std::string msg);
	static void verbose(const char* msg, ...);

	static void debug(std::string msg);
	static void debug(const char* msg, ...);

	static void info(std::string msg);
	static void info(const char* msg, ...);

	static void error(std::string err);
	static void error(const char* err, ...);

	static void printDebug(const char* fmt, ...);

private:
	Logger(LogStrategy* strategy);

	static std::unique_ptr<Logger> instance;
	LogStrategy* logStrategy;
};

#endif // DEBUGUTILS_H
