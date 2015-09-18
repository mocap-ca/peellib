#ifndef ___PEEL_WIN_UTIL_H___
#define ___PEEL_WIN_UTIL_H___
#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <string>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <malloc.h>
#include <sstream>
#include <iomanip>
#include <locale>
#include <windows.h>
#include "Wininet.h"
#include "peel/Util.h"
#include "peel/chartype.h"

using namespace std;

namespace PU
{


	STRING  windowsError(DWORD err);  //!< get the error string, use windowsError(GetLastError())
	STRING  GetExeDir();              //!< get the directory of the current executable
	bool    FileExists(STRING);       //!< confirm the file exists
	bool    FileDelete(STRING);       //!< delete a file
	bool    DirectoryExists(STRING);  //!< confirm a path is a directory
	bool    CreateDir(STRING);        //!< create a directory
	STRING  GetRegKey(STRING group, STRING key);  //!< Get a registry key in HKEY_LOCAL_MACHINE\\SOFTWARE\\Peel
	bool    SetRegKey(STRING group, STRING key, string value);  //!< Get a registry key in HKEY_LOCAL_MACHINE\\SOFTWARE\\Peel
	STRING  GetMachineId();           //!< get a string that is unique for this machine
}

#endif // WIN32
#endif // PEEL_WIN_UTIL_H