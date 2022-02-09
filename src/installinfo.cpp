#include "installinfo.h"

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
	LONG res = RegOpenKeyEx(HKEY_VMWARE, HKEY_SUBKEY_VMWARE, 0, KEY_QUERY_VALUE, &hResult);

	if (res != ERROR_SUCCESS)
	{
		throw VMWareInfoException("Couldn't open VMWare registry key. Make sure VMWare is correctly installed");
	}

	res = RegQueryValueEx(hResult, HKEY_QUERY_VALUE_INSTALLPATH, NULL, &regType, (LPBYTE)regBuf, &regSize);

	if (res != ERROR_SUCCESS)
	{
		RegCloseKey(hResult);
		throw VMWareInfoException("Couldn't read VMWare InstallPath registry value. Make sure VMWare is correctly installed");
	}

	installPath = std::string(reinterpret_cast<char*>(regBuf));

	memset(regBuf, 0, MAX_PATH);
	regSize = MAX_PATH;
	res = RegQueryValueEx(hResult, HKEY_QUERY_VALUE_INSTALLPATH64, NULL, &regType, (LPBYTE)regBuf, &regSize);

	if (res != ERROR_SUCCESS)
	{
		RegCloseKey(hResult);
		throw VMWareInfoException("Couldn't read VMWare InstallPath64 registry value. Make sure VMWare is correctly installed");
	}

	installPath64 = std::string(regBuf);

	memset(regBuf, 0, MAX_PATH);
	regSize = MAX_PATH;
	res = RegQueryValueEx(hResult, HKEY_QUERY_VALUE_PRODUCTVERSION, NULL, &regType, (LPBYTE)regBuf, &regSize);

	if (res != ERROR_SUCCESS)
	{
		RegCloseKey(hResult);
		throw VMWareInfoException("Couldn't read VMWare ProductVersion registry value. Make sure VMWare is correctly installed");
	}

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
