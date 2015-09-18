#ifndef ___SOCKET_EXCEPTION_H___
#define ___SOCKET_EXCEPTION_H___

#include "peel/chartype.h"
#include "peel/Socket.h"

namespace peel
{
	//! Thrown by Socket and HttpSocket
	class SocketException
	{
	public:
		//! Create a socket error without an error code from char string message
		SocketException(const PL_CHAR *s);

		//! Create a socket error without an error code from std::string message
		SocketException(STRING s);

		//! Create a socket error object with error code
		SocketException(STRING s, int err); 

		//! Create a socket error object including host and port data
		SocketException(STRING s, int err, STRING host, int port);

		//! Destory
		inline ~SocketException() {}; 

		//! Return the error as a string
		STRING str();

		//! Return m_str
		STRING msg();

		//! return m_err
		int error();


	   
	private:
		STRING m_str; //!< The error string
		int    m_err; //!< The error code
	};


}// namespace

#endif

