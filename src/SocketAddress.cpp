#include "peel/SocketAddress.h"

#include <string.h>

using namespace peel;

SocketAddress::SocketAddress()
{
	memset(&m_address, 0, sizeof(m_address));
}

SocketAddress::SocketAddress(SOCKET s)
{
	memset(&m_address, 0, sizeof(m_address));
 
	int l=sizeof(sockaddr_in);
#ifdef _WIN32
	getsockname(s, (sockaddr*) &m_address, &l);
#else
	getsockname(s, (sockaddr*) &m_address, (socklen_t*)&l);
#endif
}


SocketAddress::SocketAddress(struct sockaddr_in val)
: m_address(val)
{}

void SocketAddress::Set(unsigned long addr, unsigned short port)
{
        m_address.sin_family = AF_INET;
        m_address.sin_addr.s_addr = addr;
        m_address.sin_port = htons(port);
}

void SocketAddress::Clear()
{
	memset(&m_address, 0, sizeof(m_address));
}

STRING SocketAddress::GetName()
{
#if defined(_WIN32) && defined(_UNICODE)
	wchar_t wcbuf[255];
	char buf[255];
	inet_ntop(AF_INET, (void*)&(m_address.sin_addr), buf, 255);
	MultiByteToWideChar(CP_ACP, NULL, buf, -1, wcbuf, 255);
	return STRING(wcbuf);
	/*
	wchar_t wcbuf[255];
    sockaddr_in name;
	char *buf = inet_ntoa(name.sin_addr);
	MultiByteToWideChar(CP_ACP, NULL, buf, -1, wcbuf, 255);
	return STRING(wcbuf);
	*/
#else
	char buf[255];
	inet_ntop(AF_INET, (void*)&(m_address.sin_addr), buf, 255);
	return STRING(buf);
#endif


}
