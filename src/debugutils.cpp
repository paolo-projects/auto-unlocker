#include "debugutils.h"

void logd(std::string msg)
{
#ifdef VERBOSE_DBG
	std::cout << msg << std::endl;
#endif
}

void logerr(std::string err)
{
#ifdef VERBOSE_ERR
	std::cerr << "Error: " << err << std::endl;
#endif
}