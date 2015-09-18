#include "peel/SocketException.h"

using namespace peel;




SocketException::SocketException(STRING s) : m_str(s), m_err(0)
{
#ifdef _DEBUG
	COUT << _T("(debug) SocketException: ") << s << endl;
#endif
};

SocketException::SocketException(const PL_CHAR* s) :  m_str(s), m_err(0)
{
#ifdef _DEBUG
	COUT << _T("(debug) SocketException: ") << s << endl;
#endif
};


SocketException::SocketException(STRING s, int err) : m_str(s), m_err(err)
{
#ifdef _DEBUG
	COUT << _T("(debug) SocketException: ") << s << _T(" ") << err << endl;
#endif
};

SocketException::SocketException(STRING s, int err, STRING host, int port)  
{
	OSTREAM ss;
	ss << s << _T(" host: ") << host << _T(":") << port;
	m_str = ss.str();
	m_err = err;
#ifdef _DEBUG
	COUT << _T("(debug) SocketException: ") << m_str << _T(" ") << m_err << endl;
#endif
	
};


STRING SocketException::str()
{
	OSTREAM ss;
	ss << m_str;

	if(m_err!=0)
	{
#ifdef _WIN32	
		PL_CHAR buf[1024];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, m_err, 0, buf, 1024, 0);
		ss << " - " <<  buf;
#else
		ss << " - " << strerror(m_err);
#endif
	}
	return ss.str();
}

STRING SocketException::msg()
{
	return m_str;
}

int SocketException::error()
{
	return m_err;
}
