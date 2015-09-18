#include "peel/Log.h"


#include <time.h>
#include <iomanip>
using namespace std;
using std::setw;
using std::setiosflags;


namespace peel {


Log& logLock(Log &stream)
{
//	cout << "[";
	stream.waitForLock();
//#ifdef _DEBUG
//	if(stream.m_buffer.str().length() > 0)
//		cout << "BUFFER CONTAINS: " << stream.m_buffer.str() << endl;
//#endif
	return stream;
}

Log& logMsg(Log& stream)
{
	//cout << "1]";
	stream.endmsg();
	stream.unLock();
	return stream;
}

Log& logErr(Log& stream)
{
	//cout << "2]";
	stream.enderr();
	stream.unLock();
	return stream;
}

Log& logWarn(Log& stream)
{
	//cout << "3]";
	stream.endwarn();
	stream.unLock();
	return stream;
}

Log::Log()
{
	fp_stream = NULL;
#ifdef _WIN32
#ifndef DONT_USE_CRITICAL_SECTION
	InitializeCriticalSection(&cs);
#endif
#else
	pthread_mutex_init(&m_mutex, NULL);
#endif 
	m_showtime=true;

	//lock=false;
}

Log::Log(STRING outfile)
{
	fp_stream = NULL;
#ifdef _WIN32
#ifndef DONT_USE_CRITICAL_SECTION
	InitializeCriticalSection(&cs);
#endif
#else
	pthread_mutex_init(&m_mutex, NULL);
#endif

	m_showtime=true;

	//lock=false;
	Log::SetLog(outfile);
}

Log::~Log()
{
#ifdef _WIN32
#ifndef DONT_USE_CRITICAL_SECTION
	DeleteCriticalSection(&cs);
#endif
#else
	pthread_mutex_destroy(&m_mutex);
#endif
	if(fp_stream != NULL)
	{
		if(fp_stream->is_open())
			fp_stream->close();	
		delete fp_stream;
	}
}



void Log::SetLog(STRING outfile)
{
	m_filePath = outfile;
#ifdef _UNICODE
	fp_stream = new wfstream();
#else
	fp_stream = new fstream();
#endif

	if(outfile.size() == 0) return;

	fp_stream->open(outfile.c_str(), ios_base::out | ios_base::in | ios_base::trunc );// | ios_base::app);

	if(!fp_stream->is_open())
	{
		delete fp_stream;
		fp_stream=NULL;
		// cerr << "Could not open log file: " << outfile << endl;
	}
}


void Log::output(const PL_CHAR *msg, const PL_CHAR *type)
{
	waitForLock();
	_output(msg, type);
	unLock();
}

void Log::_output(const PL_CHAR *msg, const PL_CHAR *type)
{

	if(fp_stream != NULL && fp_stream->is_open())
	{
		
		if(m_showtime)
		{
			STRING time = timestr();
			*fp_stream << setiosflags(ios::left) << setw(18) << time;
		}

		*fp_stream << setiosflags (ios::left) << setw(6) <<  type;
		*fp_stream << msg << endl;
	}
	else
		COUT <<  msg << endl;	
}


void Log::write(const PL_CHAR *msg)
{
	output(msg, _T("LOG"));
}

void Log::write(STRING msg)
{
	output(msg.c_str(), _T("LOG"));
}

void Log::writehostport(const PL_CHAR *msg, const PL_CHAR *host, int port)
{
	OSTREAM ss;
	ss << msg << _T(" host:") << host << _T(" port:") << port << endl;
	write(ss.str());
}


void Log::warn(const PL_CHAR *msg)
{
	output(msg, _T("WARN"));
}

void Log::warn(STRING msg)
{
	output(msg.c_str(), _T("WARN"));
}

void Log::error(const PL_CHAR *msg)
{
	output(msg, _T("ERROR"));
}

void Log::errorcode(const PL_CHAR *msg, int i)
{
	OSTREAM ss;
	ss << msg << _T("(") << i << _T(")") << endl;
	output(ss.str().c_str(), _T("ERROR"));
}

void Log::errorhostport(const PL_CHAR *msg, const PL_CHAR *host, int port)
{
	OSTREAM ss;
	ss << msg << _T(" host:") << host << _T(" port:") << port << endl;
	output(ss.str().c_str(), _T("ERROR"));
}

void Log::info(const PL_CHAR *line, const PL_CHAR *func, const PL_CHAR *f)
{
	OSTREAM ss;
	ss << _T("line:") << line << _T(" func:") << func << _T("  f:") << f << endl;
	output(ss.str().c_str(), _T("INFO"));
}

void Log::waitForLock()
{
	//cout << "<";
#ifdef _WIN32
#ifndef DONT_USE_CRITICAL_SECTION
	EnterCriticalSection(&cs);
#endif
#else
	pthread_mutex_lock(&m_mutex);
#endif
	
};

void Log::unLock()
{
	//cout << ">" << endl;
#ifdef _WIN32
#ifndef DONT_USE_CRITICAL_SECTION
	LeaveCriticalSection(&cs);
#endif
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}

STRING Log::timestr()
{
	OSTREAM ss;
#ifdef _WIN32	
	PL_CHAR tbuf[255];
	PL_CHAR dbuf[255];
	_tstrtime(tbuf);
	_tstrdate(dbuf);
	ss << dbuf << _T(" ") << tbuf;
#else
	time_t t;
	struct tm *timeinfo;
	char buf[255];
	time(&t);
	timeinfo = localtime( &t );
	strftime(buf, 255, "%x %X ", timeinfo );
	ss << buf;
#endif
	return ss.str();
}

} // namespace peel
