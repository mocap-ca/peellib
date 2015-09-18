#ifndef ___PEEL_CHARTYPE_H__
#define ___PEEL_CHARTYPE_H__


// For windows unicode a character is two bytes unicode  (using tchar.h)
// For windows multibyte a character is one byte ascii   (using tchar.h)
// For unix a character is one byte utf8
#ifdef _WIN32
#  include <tchar.h>
#  define PL_CHAR  _TCHAR
#  define STRICMP  _tcsicmp
#  define STRCMP   _tcscmp
#  define STRLEN   _tcslen
#else
#  include <stddef.h>
#  define PL_CHAR  char
#  define STRCMP   strcmp
#  define STRICMP  strcasecmp
#  define STRLEN strlen
#  define _T(x) x
#endif


#if defined(_WIN32) && defined(_UNICODE)
#  define STRING   std::wstring
#  define OSTREAM  std::wostringstream
#  define ISTREAM  std::wistringstream
#  define OFSTREAM std::wofstream
#  define IFSTREAM std::wifstream
#  define COUT     std::wcout
#else
#  define STRING   std::string
#  define OSTREAM  std::ostringstream
#  define ISTREAM  std::istringstream
#  define OFSTREAM std::ofstream
#  define IFSTREAM std::ifstream
#  define COUT     std::cout
#endif


// These should be common for all platforms
typedef unsigned long long     pl_uint64; // u8
typedef signed long long       pl_int64;  //  8
typedef unsigned int           pl_uint32; // u4
typedef int                    pl_int32;  //  4
typedef unsigned short         pl_uint16; // u2
typedef short                  pl_int16;  //  2
typedef unsigned char          pl_byte;   // u1
typedef size_t                 pl_sizet;  // 4 on 32, 8 on 64.

// pointer to null terminated string, utf8 on unix, ascii on windows
typedef char*                  pl_char; 
typedef const char*            pl_const_char;  

// pointer to a block of data for reading and writing to/from files and sockets
typedef unsigned char          pl_data;
typedef const unsigned char   pl_const_data;



#if defined(__APPLE__) || defined(__linux__)
#  if defined(__LP64__)
#    define PL_64BIT
#  else
#    define PL_32BIT
#  endif
#else
#  if defined(__WIN64__)
#    define PL_64BIT
#  else
#    define PL_32BIT
#  endif
#endif

#if !defined(PL_64BIT) && !defined(PL_32BIT)
#  error "could not determine platform"
#endif


#endif
