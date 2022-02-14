#include "debug.h"

void Logger::init(LogStrategy* strategy)
{
	Logger::instance = std::make_unique<Logger>(strategy);
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
void Logger::printDebug(const char* fmt, ...)
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

void TerminalLogStrategy::verbose(const char* message)
{
	printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "\n", message);
}

void TerminalLogStrategy::debug(const char* message)
{
	printf(ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", message);
}

void TerminalLogStrategy::info(const char* message)
{
	printf("%s\n", message);
}

void TerminalLogStrategy::error(const char* message)
{
	fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET "\n", message);
}

StreamLogStrategy::StreamLogStrategy(std::iostream& stream)
	: stream(stream)
{
}

void StreamLogStrategy::verbose(const char* message)
{
	stream << "::VERBOSE " << message << std::endl;
}

void StreamLogStrategy::debug(const char* message)
{
	stream << "::DEBUG " << message << std::endl;
}

void StreamLogStrategy::info(const char* message)
{
	stream << "::INFO " << message << std::endl;
}

void StreamLogStrategy::error(const char* message)
{
	stream << "::ERROR " << message << std::endl;
}
