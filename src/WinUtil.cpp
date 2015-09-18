#ifdef _WIN32

#include "peel/WinUtil.h"


#include <tchar.h>

namespace PU
{


STRING GetExeDir()
{
	PL_CHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);

	STRING path = STRING(buffer);
	size_t loc = path.find_last_of(_T("\\/"));
	if(!loc)
		return STRING(_T(""));

	return path.substr(0, loc);
}




STRING windowsError(DWORD err)
{
	PL_CHAR buf[1024];
	buf[0]='\0';

	if(err!=0)
	{
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, err, 0, buf, 1024, 0);
	}
	return STRING(buf);
}

bool FileExists(STRING file)
{
	HANDLE h = CreateFile(
		file.c_str(),
		NULL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if(h == INVALID_HANDLE_VALUE)
		return false;

	CloseHandle(h);
	return true;
}

bool DirectoryExists(STRING dir)
{
	DWORD attr = GetFileAttributes(dir.c_str());
	if(attr == INVALID_FILE_ATTRIBUTES) return false;
	return (attr & FILE_ATTRIBUTE_DIRECTORY) > 0;
	
}

bool CreateDir(STRING dir)
{
	return ::CreateDirectory(dir.c_str(), NULL) == TRUE;
}

bool FileDelete(STRING file)
{
	BOOL b = DeleteFile(file.c_str());
	return b==TRUE;
}

bool SetRegKey(STRING group, STRING key, STRING value)
{	
	DWORD size = 255;
	DWORD ff = REG_SZ;

	HKEY hkey, hkey2;
	LONG lResult;
	DWORD dwSize = sizeof(DWORD);

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Peel"), 0, KEY_ALL_ACCESS, &hkey);
	if(lResult != ERROR_SUCCESS)
	{
		lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Peel"), 0, KEY_ALL_ACCESS, &hkey);
		if(lResult != ERROR_SUCCESS)
			return false;
	}

	if(RegOpenKeyEx(hkey, group.c_str(), 0, KEY_ALL_ACCESS, &hkey2)!=ERROR_SUCCESS) return false;

	if(RegSetValueEx(hkey2, key.c_str(), 0, REG_SZ, (BYTE*)value.c_str(), value.length())==ERROR_SUCCESS)
		return true;
	else
		return false;
	


}

STRING GetRegKey(STRING group, STRING key)
{
	PL_CHAR buf[255];
	DWORD size = 255;
	DWORD ff = REG_SZ;

	HKEY hkey, hkey2;
	LONG lResult;
	DWORD dwSize = sizeof(DWORD);

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Peel"), 0, KEY_QUERY_VALUE, &hkey);
	if(lResult != ERROR_SUCCESS)
	{
		lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Peel"), 0, KEY_QUERY_VALUE, &hkey);
		if(lResult != ERROR_SUCCESS) return string("");
	}

	if(RegOpenKeyEx(hkey, group.c_str(), 0, KEY_QUERY_VALUE, &hkey2)!=ERROR_SUCCESS) return string("");

	lResult = RegQueryValueEx(hkey2, key.c_str(), NULL, NULL, (LPBYTE)&buf, &size);
 	if(lResult!=ERROR_SUCCESS) return string("");

	return STRING(buf);
}

STRING GetMachineId()
{

	HKEY    hkey = 0;
	PL_CHAR buf[1024];
	DWORD   size=1023;

	/*
	if( RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion",0, KEY_QUERY_VALUE | KEY_WOW64_64KEY , &hkey) == ERROR_SUCCESS)
	{
		if(RegQueryValueExA(hkey, "ProductId", 0, NULL, (LPBYTE)&buf ,&size) == ERROR_SUCCESS)
			return std::string(buf);
	}*/

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Cryptography"), 0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hkey, _T("MachineGuid"), 0, NULL,  (LPBYTE)buf, &size) == ERROR_SUCCESS)
		{
			return STRING(buf);
		}
	}

	// 32 bit app running on a 64 bit platform
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Cryptography"), 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hkey) == ERROR_SUCCESS)
	{
		if(RegQueryValueEx(hkey, _T("MachineGuid"), 0, NULL,  (LPBYTE)&buf, &size) == ERROR_SUCCESS)
		{
			return STRING(buf);
		}
	}

	return STRING("");
}

} // namespace
#endif
