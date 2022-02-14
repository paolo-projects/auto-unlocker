#include "logging/streamlogstrategy.h"

StreamLogStrategy::StreamLogStrategy(std::ostream& stream)
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