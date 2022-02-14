#ifndef STATUSBARLOGSTRATEGY_H
#define STATUSBARLOGSTRATEGY_H

#include "logging/logstrategy.h"
#include "win32/controls/statusbar.h"

class StatusBarLogStrategy : public LogStrategy
{
public:
	StatusBarLogStrategy(StatusBar* statusBar);

	virtual void verbose(const char* message) override;
	virtual void debug(const char* message) override;
	virtual void info(const char* message) override;
	virtual void error(const char* message) override;
private:
	StatusBar* statusBar;
};

#endif