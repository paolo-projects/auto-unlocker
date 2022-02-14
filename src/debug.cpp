#include "debug.h"

Logger* Logger::instance = nullptr;

void Logger::init(LogStrategy* strategy)
{
	Logger::instance = new Logger(strategy);
}

void Logger::free()
{
	delete Logger::instance;
}

void Logger::verbose(std::string msg)
{
#if LOGLEVEL >= LOGLEVEL_VERBOSE
	Logger::instance->logStrategy->verbose(msg.c_str());
#endif
}
void Logger::verbose(const char* msg, ...)
{
#if LOGLEVEL >= LOGLEVEL_VERBOSE
	va_list args;
	va_start(args, msg);
	char fmt[1024];
	vsprintf(fmt, msg, args);

	Logger::instance->logStrategy->verbose(fmt);

	va_end(args);
#endif
}

void Logger::debug(std::string msg)
{
#if LOGLEVEL > LOGLEVEL_DEBUG
	Logger::instance->logStrategy->debug(msg.c_str());
#endif
}
void Logger::debug(const char* msg, ...)
{
#if LOGLEVEL > LOGLEVEL_DEBUG
	va_list args;
	va_start(args, msg);
	char fmt[1024];
	vsprintf(fmt, msg, args);
	
	Logger::instance->logStrategy->debug(fmt);

	va_end(args);
#endif
}

void Logger::info(std::string msg)
{
#if LOGLEVEL >= LOGLEVEL_INFO
	Logger::instance->logStrategy->info(msg.c_str());
#endif
}
void Logger::info(const char* msg, ...)
{
#if LOGLEVEL >= LOGLEVEL_INFO
	va_list args;
	va_start(args, msg);
	char fmt[1024];
	vsprintf(fmt, msg, args);

	Logger::instance->logStrategy->info(fmt);

	va_end(args);
#endif
}

void Logger::error(std::string err)
{
	Logger::instance->logStrategy->error(err.c_str());
}
void Logger::error(const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	char fmt[1024];
	vsprintf(fmt, msg, args);

	Logger::instance->logStrategy->error(fmt);

	va_end(args);
}

#ifdef _WIN32
void Logger::printWinDebug(const char* fmt, ...)
{
	char message[2048];
	va_list args;
	va_start(args, fmt);
	vsprintf(message, fmt, args);
	va_end(args);

	OutputDebugStringA(message);
}
#endif

Logger::Logger(LogStrategy* strategy)
	: logStrategy(strategy)
{

}
