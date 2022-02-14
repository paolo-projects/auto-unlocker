#include "logging/combinedlogstrategy.h"

void CombinedLogStrategy::add(LogStrategy* strategy)
{
	logStrategies.emplace_back(strategy);
}

void CombinedLogStrategy::verbose(const char* message)
{
	for (LogStrategy* strategy : logStrategies) {
		strategy->verbose(message);
	}
}

void CombinedLogStrategy::debug(const char* message)
{
	for (LogStrategy* strategy : logStrategies) {
		strategy->debug(message);
	}
}

void CombinedLogStrategy::info(const char* message)
{
	for (LogStrategy* strategy : logStrategies) {
		strategy->info(message);
	}
}

void CombinedLogStrategy::error(const char* message)
{
	for (LogStrategy* strategy : logStrategies) {
		strategy->error(message);
	}
}