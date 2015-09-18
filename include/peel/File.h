#ifndef ___PEEL_FILE_H__
#define ___PEEL_FILE_H__

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <malloc.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#endif

#include "peel/chartype.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <stdio.h>


//using namespace std;

namespace peel
{

//! File wrapper class, providing reading, writing and attribute query functions
class File
{
public:
	File();                                   //! No file
	File(const PL_CHAR* name);                //! Provide path as null termindated
	File(PL_CHAR *name, pl_sizet len);        //! Provide path and length of path string
	File(const PL_CHAR* name, pl_sizet len);  //! Const method, provide path and length of path string
	File(STRING name);                        //! Provide path
	~File();                                  //! Frees m_path if set and closes m_fp if set

	//! Open a file with specified mode
	/*! Modes are as follows:   
              'r' - read
              'w' - write
              'r+' - read/write existing
              'w+', -read/write new
 	    On windows the file is opened with CreateFile with FILE_SHARE_READ set and
	    FILE_ATTRIBUTE_NORMAL.  Other flags are set depending on read/write/
	
	    Returns true if m_fp is set - i.e. file opened successfully
 	 */
	bool open(const char *mode);     

	bool close();    //!< Close the file handle
	bool okay();     //!< Object was created sucessfuly.  Does not verify file exists, is open, etc.

#ifdef _WIN32
	bool seek(pl_uint64 offset);  //! Move the file pointer to position relative to start
#else
	bool seek(long offset);
#endif

	//! Get the position of the file pointer
	long getPosition();

	static long fileSize(const PL_CHAR *filename);  //!< get the size of a file

	void  read(pl_data* buf, pl_sizet len, pl_sizet *bytesRead = NULL);    //!< Read some data
	bool  read8(pl_data *data);          //!< read 8 bytes to data
	bool  read8(const pl_uint64 &data);  //!< read 8 bytes to data
	bool  read4(pl_data *data);          //!< read 4 bytes to data
	bool  read4(const pl_uint32 &data);  //!< read a unit32
	bool  read2(pl_data *data);          //!< read 2 bytes to data
	bool  read1(pl_data *data);          //!< read a single byte to data
	bool  read(STRING &val);             //!< read a string (stored as length:data)
	template <typename T>  bool  read(T  val);   //!< read sizeof(T) to val

	void  write(pl_data *buf, pl_sizet len, pl_sizet *bytesWritten = NULL); //! Write some data
	bool  write8(pl_data *data);         //!< write 8 bytes
	bool  write8(const pl_uint64 &data); //!< write 8 bytes
	bool  write4(pl_data *data);         //!< write 4 bytes
	bool  write4(const pl_uint32 &data); //!< write a uint32
	bool  write2(pl_data *data);         //!< write 2 bytes
	bool  write1(pl_data *data);         //!< write a single byte
	bool  write(const STRING &val);      //!< write a string (stored as length:data)
	template <typename T>  bool write(T val); //!< write sizeof(T) bytes of val

protected:

	//! the file pointer, HANDLE on windows FILE on unix, created by open()
#ifdef _WIN32
	HANDLE m_fp;
#else
	FILE*  m_fp;
#endif

	//! The local file path of the file, as defined by the constructor
	PL_CHAR *  m_path;  

};




}//namespace peel


#endif
