#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#define VERBOSE_DBG
#define VERBOSE_ERR

#include <iostream>

void logd(std::string msg);

void logerr(std::string err);

#endif // DEBUGUTILS_H
