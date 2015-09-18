#ifndef ___LOG_H_
#define ___LOG_H_

#include <string>
#include <sstream>
#include <ios>
#include <ostream>
#include <istream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "peel/chartype.h"

#ifdef  _WIN32

#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0400
#  endif

#  ifndef DONT_USE_CRITICAL_SECTION
#    include <windows.h> // for sleep and CRITICAL_SECTION
#  endif

#else
#  include "Socket.h" // for SOCKET
#endif



using namespace std;

namespace peel
{

//! Thread safe logging to a file or console.
/*!
  This class is a wrapper for logging functions, particularly for applications that
  use threads.   Define DONT_USE_CRITICAL_SECTION for non threaded apps on windows.

  All output is thread safe
*/
class Log 
{
public:
// Constructors
	//! Create a new log object that will write to stdout
	Log();                    
	//! Create a new log object that will write to a file
	Log(STRING filename);     
	//! The Destructor will close any locking objects and files
	virtual ~Log();
	//! Set the m_showtime flag
	void showtime(bool b) { m_showtime=b; }
	//! Open a file for logging.  If it fails, logging will be done to stdout
	void SetLog(STRING filename); 

// Output
	void output(const PL_CHAR* message, const PL_CHAR *type);  //!< Lock and output
	void write(STRING message);       //!< Write a "LOG" message
	void write(const PL_CHAR *message);  //!< Write a "LOG" message
	//! Write a "LOG" message, including host and port information
	/*! 
	 Will output a message in the form:  "LOG  [message] host: [host] port: [port]"
	 \param message the message
	 \param host the target host name
	 \param port the tharget port name
	*/
	void writehostport(const PL_CHAR *message, const PL_CHAR *host, int port); 
	void error(const PL_CHAR*);           //!< Write an "ERROR" message
	void errorcode(const PL_CHAR*, int);  //!< Write an "ERROR" message including a code
	void errorhostport(const PL_CHAR*, const PL_CHAR*, int); //!< Write an "ERROR" message including host and port information
	void warn(STRING);      //<! Write a "WARN" message
	void warn(const PL_CHAR*); //<! Write a "WARN" message
	void info(const PL_CHAR*, const PL_CHAR*, const PL_CHAR*);  //<! Write an "INFO" message


	//! Append m_buffer with a string
	Log& operator <<(PL_CHAR *p)
	{
		if(p == NULL)
			m_buffer << _T("(null)");
		else
			m_buffer << p; 
		return *this; 
	}

	//! Append m_buffer with a const string
	Log& operator <<(const PL_CHAR *p)
	{
		if(p == NULL)
			m_buffer << "(null)";
		else
			m_buffer << p; 
		return *this;
	}

	//! Append m_buffer with a string
	Log& operator <<(STRING s)        { m_buffer << s; return *this; }

	//! Append m_buffer with an int
	Log& operator <<(int i)           { m_buffer << i; return *this; }

	//! Append m_buffer with an 64 bit int
	Log& operator <<(pl_uint64 i)       { m_buffer << i; return *this; }

	//! Append m_buffer with a 32 bit int
	Log& operator <<(pl_uint32 i)       { m_buffer << i; return *this; }
	
	//! Append m_buffer with a 16 bit int
	Log& operator <<(pl_uint16 i)       { m_buffer << i; return *this; }

	//! Append m_buffer with an size_t


	Log& operator <<(size_t i)        { m_buffer << i; return *this; }
	//! Append m_buffer with a long

//#ifndef __APPLE__
	// on apple size_t is long unsigned int, it seems
	//Log& operator <<(long unsigned int i)          { m_buffer << i; return *this; }
//#endif



	Log& operator <<(long i)          { m_buffer << i; return *this; }


	//! Append m_buffer with a float
	Log& operator <<(float f)         { m_buffer << f; return *this; }

#ifdef _WIN32
	//! Append m_buffer with a log function (logLock, LogMsg...etc)
	Log& operator <<(Log &(__cdecl *f)(Log &))  { return f(*this);}
#else
	//! Append m_buffer with a log function (logLock, LogMsg...etc)
	Log& operator <<(Log &(*f)(Log &))  { return f(*this);}
#endif

	Log& operator <<(void*i)         { m_buffer << i; return *this; }

	void waitForLock(); //!< Block for another thread if needed
	void unLock();      //!< Release lock on mutex/critical section letting waitForLock to unblock
	OSTREAM m_buffer;  //!< Buffer for log << "String" operations

	// Utility
	STRING timestr();   //!< get a time string
	STRING m_filePath;  //!< Filename to write to.  
	//! File stream to write to.
	/*!
	  If this fstream object is null or not open, writing will be done to the console
	  instead.  See the code for _output() for specifics.
	*/
	
#ifdef _UNICODE
	wfstream *fp_stream; 
#else
	fstream *fp_stream; 
#endif

public:

	
	//! Override this method to perform actual message output operation, otherwise() _output will be called
	virtual void _output_msg(const PL_CHAR *message)   { _output(message, _T("INFO"));  }

	//! Override this method to perform actual error output operation, otherwise _output() will be called
	virtual void _output_err(const PL_CHAR *message)   { _output(message, _T("ERROR")); }

	//! Override this method to perform actual warning output operation, otherwise _output() will be called
	virtual void _output_warn(const PL_CHAR *message)  { _output(message, _T("WARN"));  }

	//! Write buffer as information - calls _output_msg
	void endmsg() { _output_msg(m_buffer.str().c_str()); m_buffer.str(_T(""));  } 

	//! Write buffer as error - calls _output_err
	void enderr() { _output_err(m_buffer.str().c_str()); m_buffer.str(_T("")); } 

	//! Write buffer as warning - calls _output_warn
	void endwarn() { _output_warn(m_buffer.str().c_str()); m_buffer.str(_T("")); } 

private:
	//! Perform acutal output operation
	/*! Perform output operation for other _ouput_* methods - will output to fp_stream if it is set and open,
 	    otherwise it will write to COUT
	*/
	void _output(const PL_CHAR *message, const PL_CHAR *type); 
	

#ifdef _WIN32
#ifndef DONT_USE_CRITICAL_SECTION
	CRITICAL_SECTION cs; //!< For windows locking
#endif
#else
	pthread_mutex_t  m_mutex; //!< For unix locking
#endif

	//! Show time data flag
	/*! 
	  This  will be set to true by showtime(bool) if the time and date information
	  should be shown on the output
	*/
	bool    m_showtime;  

};

Log& logLock(Log &stream);  //!< Lock the log object for writing
Log& logMsg(Log& stream);   //!< Write contents of m_buffer using write()
Log& logErr(Log &stream);   //!< Write contents of m_buffer using error()
Log& logWarn(Log &stream);  //!< Write contents of m_buffer using warn()


} // namespace peel

#endif
