#ifndef TERMINALLOGSTRATEGY_H
#define TERMINALLOGSTRATEGY_H

#include <stdio.h>
#include "colors.h"
#include "logging/logstrategy.h"

class TerminalLogStrategy : public LogStrategy
{
public:
	virtual void verbose(const char* message) override;
	virtual void debug(const char* message) override;
	virtual void info(const char* message) override;
	virtual void error(const char* message) override;
};

#endif