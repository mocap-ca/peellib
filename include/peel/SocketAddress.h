#ifndef PEEL_SOCKET_ADDRESS_H
#define PEEL_SOCKET_ADDRESS_H


#include "chartype.h"
#include "SocketDefines.h"
#include <string>

namespace peel 
{

class SocketAddress
{
public:
	SocketAddress();
	SocketAddress(SOCKET s);
	SocketAddress(struct sockaddr_in);

	void   Clear();
	void   Set(unsigned long addr, unsigned short port);
	STRING GetName();

	struct sockaddr_in m_address;

	operator sockaddr* () { return (sockaddr*)&m_address; }
};

}

#endif
