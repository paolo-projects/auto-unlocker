#ifndef COMBINEDLOGSTRATEGY_H
#define COMBINEDLOGSTRATEGY_H

#include <vector>
#include "logging/logstrategy.h"

class CombinedLogStrategy : public LogStrategy
{
public:
	CombinedLogStrategy() {};

	void add(LogStrategy* strategy);

	virtual void verbose(const char* message) override;
	virtual void debug(const char* message) override;
	virtual void info(const char* message) override;
	virtual void error(const char* message) override;
private:
	std::vector<LogStrategy*> logStrategies;
};

#endif