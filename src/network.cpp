#include "network.h"

Network::Network() {
	CURLcode res = curl_global_init(CURL_GLOBAL_ALL);

	if (res != CURLE_OK) {
		throw NetworkException("Could not initialize CURL", res);
	}
}

Network::~Network() {
	curl_global_cleanup();
}

void Network::setProgressCallback(std::function<void(float)> progressCallback)
{
	this->progressCallback = progressCallback;
}

/**
	Writes data received from Curl to a stream
*/
size_t Network::write_data_file(char* ptr, size_t size, size_t nmemb, void* stream)
{
	std::iostream* fstr = static_cast<std::iostream*>(stream);
	size_t bytes = size * nmemb;
	fstr->write(ptr, bytes);
	return bytes;
}

int Network::progress_callback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	NetworkProgress* networkProgress = reinterpret_cast<NetworkProgress*>(clientp);

	if (dltotal > 0)
	{
		double mBytesTotal = dltotal / 1024 / 1024;
		double mBytesNow = dlnow / 1024 / 1024;

		long long nowTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		// Drop progress update if less than a set time has elapsed since the last one
		if (nowTime - networkProgress->lastProgressUpdateTime < updatePeriodMs)
		{
			// Don't update too frequently.
			return 0;
		}

		double mBytesDelta = mBytesNow - networkProgress->mBytesDownloadedLastTime;
		long long timeDeltaMs = nowTime - networkProgress->lastProgressUpdateTime;
		double downloadRate = mBytesDelta / (timeDeltaMs / 1000.);

		printf("Download progress: %d %%, %.2f MB / %.2f MB, %.3f MB/s          \r",
			(std::min)(100, (std::max)(0, int(dlnow * 100 / dltotal))),
			mBytesNow, mBytesTotal, downloadRate);
		fflush(stdout);

		networkProgress->mBytesDownloadedLastTime = mBytesNow;
		networkProgress->lastProgressUpdateTime = nowTime;
	}
	return 0;
}

int Network::progress_callback_external(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	Network* instance = reinterpret_cast<Network*>(clientp);
	instance->progressCallback(dlnow / dltotal);
	return 0;
}

void Network::curlDownload(const std::string& url, const std::string& fileName)
{
	CURL* curl = curl_easy_init();
	if (curl)
	{
		std::fstream out_file(fileName, std::ios::out | std::ios::binary);
		/* set URL to get here */
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

#if CURL_DEBUG
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		/* disable progress meter, set to 0L to enable and disable debug output */
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
#endif

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Network::write_data_file);
		/* write the page body to this ofstream */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);
		/* if response is >400, return an error */
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		/* progress monitoring function */
		if (progressCallback != nullptr) 
		{
			curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, this);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Network::progress_callback_external);
		}
		else 
		{
			curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &networkProgress);
			curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Network::progress_callback);
		}
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		/* get it! */
		networkProgress.mBytesDownloadedLastTime = 0.0;
		networkProgress.lastProgressUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		CURLcode res = curl_easy_perform(curl);
		out_file.close();

		printf("\n");

		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			throw NetworkException("Error in CURL get request: " + std::string(curl_easy_strerror(res)), res);
		}
	}
	else throw std::runtime_error("Could not initialize CURL instance");
}

std::string Network::curlGet(const std::string& url)
{
	CURL* curl = curl_easy_init();
	if (curl)
	{
		std::stringstream sstream;

		/* set URL to get here */
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

#if CURL_DEBUG
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		/* disable progress meter, set to 0L to enable and disable debug output */
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
#endif

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Network::write_data_file);
		/* write the page body to the stringstream */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sstream);
		/* if response is >400, return an error */
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		/* progress monitoring function */
		/*curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);*/
		/* get it! */
		CURLcode res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);

		if (res != CURLE_OK)
		{
			throw NetworkException("Error in CURL get request: " + std::string(curl_easy_strerror(res)), res);
		}

		return sstream.str();
	}
	else throw std::runtime_error("Could not initialize CURL instance");
}
