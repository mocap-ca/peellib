#include "peel/Util.h"


// for stat
#include <sys/types.h>
#include <sys/stat.h> 

#ifdef _WIN32
#include <tchar.h>
#endif

namespace PU
{


static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
//static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

void b64_encode(  char in[3],  char out[4], int len ){
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

bool TimeStr(time_t time, std::string &res)
{
	std::ostringstream tss;

	if(time>=86400)
	{
		tss << std::setw(2) << std::setfill('0') << (time / 86400) << ":";
		time = time % 86400;
	}				
	if(time>=3600)
	{
		tss << std::setw(2) << std::setfill('0') <<  (time / 3600) << ":";
		time = time % 3600;
	}
	if(time>=60)
	{
		tss << std::setw(2) << std::setfill('0') <<  (time / 60) << ":";
		time = time % 60;
	}		
	tss << std::setw(2) << std::setfill('0') << time;

	res = tss.str();

	return true;

}

std::string UpperCase(const std::string& s)
{
    std::string result = s;
    std::transform<std::string::iterator,
                   std::string::iterator,
                   int (*)(int)>(result.begin(), result.end(), 
                     result.begin(), ::toupper);
    return result;
}


bool StrICmp(STRING a, STRING b)
{
	if(STRICMP(a.c_str(), b.c_str())==0)
		return true;
	else
		return false;
}
	

bool StrICmp(STRING a, const PL_CHAR* b)
{
	if(STRICMP(a.c_str(), b)==0)
		return true;
	else
		return false;
}

bool isNumeric(STRING &s)
{
	size_t i=0;
	if(s.size()==0) return false;
	return (s.find_first_not_of(_T("0123456789"),i)==string::npos);
}

bool UrlEncode(const string & Url, std::string &result)
{
	//std::cout << "Url "<< Url << std::endl;
	
	//char *tmp = new char [(Url.length()*3)];
	char *tmp = new char[(Url.length()*3)+1];

	char t[10];
	int j=0;

	for (size_t i = 0; i < Url.length(); i++)
	{
		if (isalnum(Url[i]))
			tmp[j++]=Url[i];
		else
		{
#ifdef _WIN32   
			sprintf_s(t, 10, "%%%02X", Url[i]);
			tmp[j]=0;
			strcat_s(tmp, (Url.length()*3)+1, t);
#else
			sprintf(t, "%%%02X", Url[i]);
			tmp[j]=0;
			strcat(tmp, t);
#endif
			j+=strlen(t);
		}
	}
	tmp[j]=0;
	result = tmp;

	//std::cout << "newUrl "<<newUrl << std::endl;
	delete[] tmp;
	return true;
}

bool UrlDecode(const std::string & Url, std::string &res)
{
	int length = Url.length();  // get the length of the string
	int offset = 0; // difference between original and new length
	int i; // count
	// go through each character of the string
	for (i=0;i<length;i++) {
		// for every % that you find, increase the offset by two.
		if (Url.c_str()[i] == '%') offset += 2;
	}
  
	// create a new string of the corrent length.
	char *result = new char[length-offset+1];
	// create a pointer to that string
	char *current = result;
	// go through each character of the original string
	for (i=0; i<length; i++, current++ ) {
		switch (Url.c_str()[i]) {
			// if the current character is a %, send the string starting from
			// the next character to be decoded from hexadecimal.
		case '%':
			*current = UrlHexToChar(&Url[i+1]);
			// increment the count to compensate for the values passed over
			i+=2;
			break;
			// if the current character is a +, replace it with a space.
		case '+':
			*current = ' ';
			break;
			// if the current character is anything else, copy it over
		default:
			*current = Url[i];
		}
	}
	// set the final character to NULL to make a null terminated string
	*current = 0;
	// return the result.
	res = result;

	delete[] result;

	return true;
}


char UrlHexToChar(const char * pChars)
{
    int Hi;        // holds high byte
    int Lo;        // holds low byte
    char result;    // holds result

    // Get the value of the first byte to Hi

    Hi = pChars[0];
    if ('0' <= Hi && Hi <= '9')
        Hi -= '0';
    else
    if ('a' <= Hi && Hi <= 'f')
        Hi -= ('a'-10);
    else
    if ('A' <= Hi && Hi <= 'F')
        Hi -= ('A'-10);

    // Get the value of the second byte to Lo

    Lo = pChars[1];
    if ('0' <= Lo && Lo <= '9')
        Lo -= '0';
    else
    if ('a' <= Lo && Lo <= 'f')
        Lo -= ('a'-10);
    else
    if ('A' <= Lo && Lo <= 'F')
        Lo -= ('A'-10);

    result = (char)(Lo + (16 * Hi));
    return result;


}


STRING PCSlash(STRING src)
{
	STRING dest;
	for(size_t i=0; i< src.length(); i++)
	{
		if(src[i]!=_T('/'))
			dest.push_back(src[i]);
		else
			dest.push_back(_T('\\'));
	}
	return dest;
}


int fileSize(const STRING &file)
{
#ifdef _WIN32
	struct _stat  st;

	if(_tstat(file.c_str(), &st) != 0) return -1;
	else return st.st_size;
#else
	struct stat st;
	if(stat(file.c_str(), &st) != 0) return -1;
	else return st.st_size;
#endif
}


}// namespace
