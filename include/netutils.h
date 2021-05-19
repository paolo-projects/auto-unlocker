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

#define DEBUG false

namespace Curl
{
	size_t write_data_file(char* ptr, size_t size, size_t nmemb, void* stream);
	int progress_callback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);
	CURLcode curlDownload(std::string url, std::string fileName);
	CURLcode curlGet(std::string url, std::string& output);
}

#endif // NETUTILS_H
