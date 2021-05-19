#include "netutils.h"

/**
	Writes data received from Curl to a stream
*/
size_t Curl::write_data_file(char* ptr, size_t size, size_t nmemb, void* stream)
{
	std::iostream* fstr = static_cast<std::iostream*>(stream);
	size_t bytes = size * nmemb;
	fstr->write(ptr, bytes);
	return bytes;
}

constexpr double mBytesProgressUpdateDelta = 0.1; // 100 kB
static double mBytesDownloadedLastTime = 0.0;

int Curl::progress_callback(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (dltotal > 0)
	{
		double mBytesTotal = dltotal / 1024 / 1024;
		double mBytesNow = dlnow / 1024 / 1024;

		if ((mBytesNow - mBytesDownloadedLastTime) < mBytesProgressUpdateDelta)
		{
			// Don't update too frequently.
			return 0;
		}

		std::cout << "Download progress: " << (std::min)(100, (std::max)(0, int(dlnow * 100 / dltotal))) << " %, " << std::fixed << std::setprecision(2) << mBytesNow << " MB / " << mBytesTotal << " MB                    \r" << std::flush;

		mBytesDownloadedLastTime = mBytesNow;
	}
	return 0;
}

CURLcode Curl::curlDownload(std::string url, std::string fileName)
{
	CURL* curl = curl_easy_init();
	if (curl)
	{
		std::fstream out_file(fileName, std::ios::out | std::ios::binary);
		/* set URL to get here */
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		if (DEBUG)
		{
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			/* disable progress meter, set to 0L to enable and disable debug output */
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		}

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_file);
		/* write the page body to this ofstream */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out_file);
		/* if response is >400, return an error */
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		/* progress monitoring function */
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		/* get it! */
		mBytesDownloadedLastTime = 0.0;
		CURLcode res = curl_easy_perform(curl);
		out_file.close();

		std::cout << std::endl;

		curl_easy_cleanup(curl);

		return res;
	}
	else return CURLE_FAILED_INIT;
}

CURLcode Curl::curlGet(std::string url, std::string& output)
{
	CURL* curl = curl_easy_init();
	if (curl)
	{
		std::stringstream sstream;

		/* set URL to get here */
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		if (DEBUG)
		{
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			/* disable progress meter, set to 0L to enable and disable debug output */
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		}

		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_file);
		/* write the page body to the stringstream */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sstream);
		/* if response is >400, return an error */
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);
		/* progress monitoring function */
		/*curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);*/
		/* get it! */
		CURLcode res = curl_easy_perform(curl);
		output = sstream.str();

		curl_easy_cleanup(curl);

		return res;
	}
	else return CURLE_FAILED_INIT;
}
