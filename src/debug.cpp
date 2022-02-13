#include "debug.h"

#ifdef _WIN32
WORD defAttrs;
bool hasDefAttrs = false;
#endif

void logv(std::string msg)
{
#if LOGLEVEL >= LOGLEVEL_VERBOSE
#ifdef _WIN32
	setTerminalColor(FOREGROUND_RED | FOREGROUND_GREEN);
	std::cout << msg;
	resetTerminalColor();
	std::cout << std::endl;
#else
	std::cout << ANSI_COLOR_BLUE << msg << ANSI_COLOR_RESET << std::endl;
#endif
#endif
}
void logv(const char* msg, ...)
{
#if LOGLEVEL >= LOGLEVEL_VERBOSE
	va_list args;
	va_start(args, msg);
	char fmt[1024];
	vsprintf(fmt, msg, args);

#ifdef _WIN32
	setTerminalColor(FOREGROUND_RED | FOREGROUND_GREEN);
	printf("%s", fmt);
	resetTerminalColor();
	printf("\n");
#else
	printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "\n", fmt);
#endif

	va_end(args);
#endif
}

void logd(std::string msg)
{
#if LOGLEVEL > 0
#ifdef _WIN32
	setTerminalColor(FOREGROUND_BLUE | FOREGROUND_GREEN);
	std::cout << msg;
	resetTerminalColor();
	std::cout << std::endl;
#else
	std::cout << ANSI_COLOR_CYAN << msg << ANSI_COLOR_RESET << std::endl;
#endif
#endif
}
void logd(const char* msg, ...)
{
#if LOGLEVEL > 0
	va_list args;
	va_start(args, msg);
	char fmt[1024];
	vsprintf(fmt, msg, args);

#ifdef _WIN32
	setTerminalColor(FOREGROUND_BLUE | FOREGROUND_GREEN);
	printf("%s", fmt);
	resetTerminalColor();
	printf("\n");
#else
	printf(ANSI_COLOR_CYAN "%s" ANSI_COLOR_RESET "\n", fmt);
#endif

	va_end(args);
#endif
}

void logi(std::string msg)
{
#if LOGLEVEL >= LOGLEVEL_INFO
	std::cout << msg << std::endl;
#endif
}
void logi(const char* msg, ...)
{
#if LOGLEVEL >= LOGLEVEL_INFO
	va_list args;
	va_start(args, msg);
	char fmt[1024];
	vsprintf(fmt, msg, args);

	printf("%s\n", fmt);

	va_end(args);
#endif
}

void logerr(std::string err)
{
#ifdef _WIN32
	setTerminalColor(FOREGROUND_RED, true);
	std::cerr << err;
	resetTerminalColor(true);
	std::cerr << std::endl;
#else
	std::cerr << ANSI_COLOR_RED << err << ANSI_COLOR_RESET << std::endl;
#endif
}
void logerr(const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	char fmt[1024];
	vsprintf(fmt, msg, args);

#ifdef _WIN32
	setTerminalColor(FOREGROUND_RED, true);
	fprintf(stderr, "%s", fmt);
	resetTerminalColor(true);
	fprintf(stderr, "\n");
#else
	fprintf(stderr, ANSI_COLOR_RED "%s" ANSI_COLOR_RESET "\n", fmt);
#endif

	va_end(args);
}

#ifdef _WIN32
void setTerminalColor(WORD tAttribute, bool isStderr)
{
	if (!hasDefAttrs)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO bInfo = {};
		GetConsoleScreenBufferInfo(hConsole, &bInfo);
		defAttrs = bInfo.wAttributes;
		hasDefAttrs = true;
	}

	WORD newAttrs = defAttrs & ~FOREGROUND_GREEN & ~FOREGROUND_BLUE & ~FOREGROUND_RED;
	newAttrs |= tAttribute;
	HANDLE hConsole = GetStdHandle(isStderr ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, newAttrs);
}

void resetTerminalColor(bool isStderr)
{
	if (hasDefAttrs) {
		HANDLE hConsole = GetStdHandle(isStderr ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, defAttrs);
	}
}
#endif