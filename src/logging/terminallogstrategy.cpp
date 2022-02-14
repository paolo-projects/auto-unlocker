#include "logging/terminallogstrategy.h"

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