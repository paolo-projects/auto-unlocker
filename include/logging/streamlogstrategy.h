#ifndef STREAMLOGSTRATEGY_H
#define STREAMLOGSTRATEGY_H

#include <fstream>
#include "logging/logstrategy.h"

class StreamLogStrategy : public LogStrategy
{
public:
	StreamLogStrategy(std::ostream& stream);

	virtual void verbose(const char* message) override;
	virtual void debug(const char* message) override;
	virtual void info(const char* message) override;
	virtual void error(const char* message) override;
private:
	std::ostream& stream;
};

#endif