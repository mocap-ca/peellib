#ifndef ___UTIL_H_
#define ___UTIL_H_

#include <string>
#include <string.h>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#ifdef __APPLE__
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include <sstream>
#include <sys/timeb.h>
#include <iomanip>
#include <time.h>
#include <locale>
#include "peel/chartype.h"


using namespace std;

namespace PU
{
	bool  TimeStr(time_t time, string &res);          //!< convert time_t to string
        bool  UrlEncode(const string & Url, string &res); //!< url encode a string
        bool  UrlDecode(const string & Url, string &res); //!< url decode a string
	string  UpperCase(const STRING&);      //!< Make a string uppercase
	bool    StrICmp(STRING , STRING);       //!< Case insensitive compare
	bool    StrICmp(STRING , const PL_CHAR *);  //!< Case insensitive compare
	bool    isNumeric(STRING&);            //!< All numbers only?
	char    UrlHexToChar(const char * pChars);  
	void    b64_encode( char in[3], char out[4], int len );
	STRING  PCSlash(STRING);               //!< Swap all '/' to '\'
	int     fileSize(const STRING &file);  //!< Get the size of a file (uses stat)
};


#endif

