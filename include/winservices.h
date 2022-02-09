#ifndef SERVICESTOPUTILS_H
#define SERVICESTOPUTILS_H

#include <string>
#include <stdarg.h>

#ifdef _WIN32
#include "Windows.h"
#else
typedef int SC_HANDLE; // dummy typename for non windows systems
#endif

class ServiceStopper
{
public:
	class ServiceStopException : public std::exception
	{
	public:
		ServiceStopException(const char* msg, ...) {
			char formatted[512];
			va_list args;
			va_start(args, msg);
			vsprintf(formatted, msg, args);
			va_end(args);
			sprintf(message, "Error: %s", formatted);
		}
		const char* what() const noexcept { return message; }
	private:
		char message[1024];
	};

	// TODO: check if linux needs these functions and if so reimplement them
	//		 or alternatively wrap them in ifdef if it doesn't need them
	static void StopService_s(std::string serviceName);
	static bool StopDependantServices(SC_HANDLE schService, SC_HANDLE schSCManager);

	static bool StartService_s(std::string serviceName);

	static bool KillProcess(std::string procName);
};

#endif // SERVICESTOPUTILS_H
