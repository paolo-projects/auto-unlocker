#ifndef EVENTS_H
#define EVENTS_H

#include <functional>

enum class EventType {
	CLICK_EVENT
};

using EventCallback = std::function<void(void)>;

#endif