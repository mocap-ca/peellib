#ifndef ___PEEL_UNIX_UTIL_H___
#define ___PEEL_UNIX_UTIL_H___

#include <string>
#include <string.h>
#include <stdio.h>
#include <iostream>

#ifdef __APPLE__
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include <sstream>
#include <iomanip>
#include <locale>
#include "peel/Util.h"

using namespace std;

namespace PU
{
	string  unixError(int err);
	bool    FileExists(string);
	bool    DirectoryExists(string);
	bool    FileDelete(string);
}

#endif // PEEL_UNIX_UTIL_H
