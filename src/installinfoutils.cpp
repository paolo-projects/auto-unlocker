#include "installinfoutils.h"

#ifdef _WIN32
#include "Windows.h"
#endif

VMWareInfoRetriever::VMWareInfoRetriever()
{
#ifdef _WIN32
	char regBuf[MAX_PATH];
	memset(regBuf, 0, MAX_PATH);

	DWORD regSize = MAX_PATH;
	DWORD regType = 0;

	HKEY hResult;
	RegOpenKeyEx(HKEY_VMWARE, HKEY_SUBKEY_VMWARE, 0, KEY_QUERY_VALUE, &hResult);

	RegQueryValueEx(hResult, HKEY_QUERY_VALUE_INSTALLPATH, NULL, &regType, (LPBYTE)regBuf, &regSize);
	installPath = std::string(reinterpret_cast<char*>(regBuf));

	memset(regBuf, 0, MAX_PATH);
	regSize = MAX_PATH;
	RegQueryValueEx(hResult, HKEY_QUERY_VALUE_INSTALLPATH64, NULL, &regType, (LPBYTE)regBuf, &regSize);
	installPath64 = std::string(regBuf);

	memset(regBuf, 0, MAX_PATH);
	regSize = MAX_PATH;
	RegQueryValueEx(hResult, HKEY_QUERY_VALUE_PRODUCTVERSION, NULL, &regType, (LPBYTE)regBuf, &regSize);
	prodVersion = std::string(regBuf);
	
	RegCloseKey(hResult);
#endif
}

std::string VMWareInfoRetriever::getInstallPath()
{
	return installPath;
}

std::string VMWareInfoRetriever::getInstallPath64()
{
	return installPath64;
}

std::string VMWareInfoRetriever::getProductVersion()
{
	return prodVersion;
}
