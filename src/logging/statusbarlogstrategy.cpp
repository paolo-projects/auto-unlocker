#include "logging/statusbarlogstrategy.h"

StatusBarLogStrategy::StatusBarLogStrategy(StatusBar* statusBar)
	: statusBar(statusBar)
{

}

void StatusBarLogStrategy::verbose(const char* message)
{
	statusBar->setText(message);
}

void StatusBarLogStrategy::debug(const char* message)
{
	statusBar->setText(message);
}

void StatusBarLogStrategy::info(const char* message)
{
	statusBar->setText(message);
}

void StatusBarLogStrategy::error(const char* message)
{
	statusBar->setText(message);
}