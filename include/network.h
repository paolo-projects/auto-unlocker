#ifndef NETUTILS_H
#define NETUTILS_H

#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <functional>

#define CURL_DEBUG false

class NetworkException : public std::runtime_error
{
public:
	NetworkException(const char* message, CURLcode code) : std::runtime_error(message), code(code) {}
	NetworkException(const std::string& message, CURLcode code) : std::runtime_error(message), code(code) {}
	CURLcode getCode() const { return code; }
private:
	CURLcode code;
};

struct NetworkProgress {
	double mBytesDownloadedLastTime = 0.0;
	long long lastProgressUpdateTime = 0;
};

class Network
{
public:
	Network();
	~Network();
	void curlDownload(const std::string& url, const std::string& fileName);
	std::string curlGet(const std::string& url);
private:
	static constexpr double mBytesProgressUpdateDelta = 0.1; // 0.1 MB
	static constexpr long long updatePeriodMs = 200; // update every 100ms

	NetworkProgress networkProgress = {};
	static size_t write_data_file(char* ptr, size_t size, size_t nmemb, void* stream);
	static int progress_callback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);
};

#endif // NETUTILS_H
