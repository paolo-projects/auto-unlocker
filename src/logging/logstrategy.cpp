#ifndef LOGSTRATEGY_H
#define LOGSTRATEGY_H

class LogStrategy
{
public:
	virtual void verbose(const char* message) = 0;
	virtual void debug(const char* message) = 0;
	virtual void info(const char* message) = 0;
	virtual void error(const char* message) = 0;
};

#endif