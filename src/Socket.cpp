

#include "peel/Socket.h"
#include "peel/SocketException.h"
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#endif


using namespace peel;


#ifdef _WIN32

WSADATA peel::gWsaData;

bool peel::SocketsInit()
{
	int i = WSAStartup(MAKEWORD(2,2), &peel::gWsaData);
	return i == 0;
}


bool peel::SocketsCleanup()
{
	return WSACleanup() == 0;
}

#endif


peel::Socket::Socket()
: m_socket(SOCKET_NULL)
, m_socketType(peel::TCP)
{
	CreateSocket(peel::TCP);
}

Socket::Socket(SOCKET s)
: m_socket(s)
, m_socketType(peel::INVALID)
{
	// TODO: set m_socketType
}

peel::Socket::Socket(TSocketType type)
: m_socket(SOCKET_NULL)
, m_socketType(type)
{
	CreateSocket(type);
}	


Socket::~Socket()
{
	try
	{
		if (m_socket != SOCKET_NULL && m_socket != INVALID_SOCKET) DestroySocket();
	}
	catch (SocketException e)
	{
	}
}


// Socket Creation/Destruction


void Socket::CreateSocket(int socketType)
{
	if(m_socket!=SOCKET_NULL && m_socket != INVALID_SOCKET) DestroySocket();

	switch(socketType)
	{
		case TCP :
			m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			break;

		case UDP:
			m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			break;

		case UNIX:
			m_socket = socket(AF_UNIX, SOCK_STREAM, 0);
			break;

		default:
			throw SocketException(_T("Invalid socket type requested"));
	}

	if(m_socket==SOCKET_NULL || m_socket == INVALID_SOCKET)
		throw SocketException(_T("Error creating socket: "), GetError());	



#ifdef _OSX
	int set = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
#endif
}

void Socket::DestroySocket()
{
	if(m_socket==SOCKET_NULL)     throw SocketException(_T("Attempting to close null socket"));
	if(m_socket==INVALID_SOCKET)  throw SocketException(_T("Attempting to close invalid socket"));
	if(CLOSESOCKET(m_socket)!=0)  throw SocketException(_T("Could not close socket"), GetError());
	m_socket=SOCKET_NULL;
}



// Information

STRING Socket::GetName()
{
	SocketAddress addr(m_socket);
	return addr.GetName();
}


STRING Socket::GetPeer()
{
	struct sockaddr_in peer;
	int l=sizeof(sockaddr_in);

#ifdef _WIN32
	int ret = getpeername(m_socket, (sockaddr*) &peer, &l);
#else
	int ret = getpeername(m_socket, (sockaddr*) &peer, (socklen_t*)&l);
#endif
	
	if(ret != 0) throw SocketException(_T("Could not get peer"), GetError());
	
	SocketAddress addr(peer);
	return addr.GetName();
}


bool Socket::LookupAddress (const PL_CHAR* pcHost, void *ret)
{
	// ret must be sizeof(struct in_addr) sized.

#if defined(_WIN32) && defined(_UNICODE)
	ADDRINFOT *result = NULL;
	GetAddrInfo(pcHost, NULL, NULL, &result);
	ret = (u_long)result->ai_addr;
	memcpy(ret, result->ai_addr, sizeof(ulong));
	return true;
#else
	unsigned char nRemoteAddr[sizeof(struct in_addr)];
	int r = inet_pton(AF_INET, pcHost, &nRemoteAddr);
	if (r <= 0) 
	{/*
		hostent* pHE = gethostbyname(pcHost);  // pcHost isn't a dotted IP, so resolve it through DNS
		if (pHE == 0)  return INADDR_NONE;
		nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);*/
	}
	memcpy(ret, nRemoteAddr, sizeof(struct in_addr));
	return true;
#endif
}



int Socket::GetError()
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}


STRING Socket::GetComputerName()
{
	char temp[256];
	int ret = gethostname(temp,256);
	if(ret == SOCKET_ERROR)
		throw(SocketException(_T("Could not get computer name"),GetError()));

#ifdef _WIN32
#ifdef _UNICODE
	wchar_t wcbuf[256];
	MultiByteToWideChar(CP_ACP, 0, temp, -1, wcbuf, 255);
	return wstring(wcbuf);
#else
	return string(temp);
#endif
#else
	return STRING(temp);
#endif
}

void Socket::SetBlocking(bool block)
{
#ifdef _WIN32
	u_long arg;
	if(block)
		arg = 0;
	else
		arg = 1;
	ioctlsocket(m_socket, FIONBIO, &arg);
#else
	int x;
	x=fcntl(m_socket,F_GETFL,0);
	if(block)
		fcntl(m_socket,F_SETFL,x &= ~O_NONBLOCK);
	else
		fcntl(m_socket,F_SETFL,x | O_NONBLOCK);	
#endif
/*
	int nonblocking = block ? 0 : 1;
	if (ioctl(s, FIONBIO, &on) < 0) {
        throw SocketException("Could not set socket blocking mode");
*/

}


/*****************************************
*             broadcast
*****************************************/

void Socket::SetBroadcast(bool broadcast)
{
	if(m_socketType != UDP)
        throw SocketException(_T("Invalid Socket Type while trying to set broadcast"));

	int b = broadcast ? 1 : 0;
#ifdef _WIN32
	if(setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (const char*) &b, sizeof(b)) < 0)
		throw SocketException(_T("Could not set socket broadcast mode"), errno);
#else
	if(setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (void*) &b, sizeof(b)) < 0)
		throw SocketException("Could not set socket broadcast mode", errno);
#endif
}

void Socket::SetDestination(const char *ip, unsigned short port)
{
    if(m_socketType != UDP) throw SocketException(_T("Invalid Socket Type while trying to set destination"));
	unsigned long addrBuf;
	inet_pton(AF_INET, ip, &addrBuf);
	m_sendDest.Set(addrBuf, port);
}

void Socket::SetDestination(struct sockaddr_in &in, unsigned short port)
{
    if(m_socketType != UDP) throw SocketException(_T("Invalid Socket Type while trying to set destination"));
	m_sendDest.Set(in.sin_addr.s_addr, port);
}
	

void Socket::SendTo(const void *data, size_t len)
{
    if(m_socketType != UDP) throw SocketException(_T("Invalid Socket Type while trying to send datagram"))
;
	size_t ret = sendto(m_socket, (const char*)data, (int)len, 0,
				 (struct sockaddr *)&m_sendDest.m_address,
				 (int)sizeof(m_sendDest.m_address));

	if(ret == (size_t)-1) throw SocketException(_T("Error sending broadcast message"), GetError());
	else if(ret != len)   throw SocketException(_T("Not all bytes were sent during broadcast message"), GetError());
}


//******************************************
//		client stuff		
//******************************************
#if defined(_WIN32) 
void Socket::Connect(const PL_CHAR *host, unsigned short port)
{

	// This is a windows unicode specific implementation which uses 
	// GetAttrInfo to populate result with a number of possible results
	// for which each are tried to connect to.
	if(m_socket==SOCKET_NULL || m_socket == INVALID_SOCKET)
		throw SocketException(_T("Invalid socket while trying to connect"));

    OSTREAM ossport;
	ossport << (int)port;

	ADDRINFOT hints;
	ZeroMemory( &hints, sizeof(hints) );

	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	ADDRINFOT *result = NULL;

	if(GetAddrInfo(host, ossport.str().c_str(), &hints, &result)!= 0)
		throw SocketException(_T("Could not find host"));

	ADDRINFOT *current = result;

	bool okay = false;

	while(1)
	{
		if ( connect( m_socket, current->ai_addr, (int)current->ai_addrlen ) != SOCKET_ERROR) 
		{
			okay = true;
			break;
		}
		if(result->ai_next == NULL) break;
		current = result->ai_next;
	}

	if(!okay)
	{
		m_socket = INVALID_SOCKET;
		throw SocketException(_T("Failed to connect"), GetError(), host, port);
	}
	//	delete hp?
	return;

}

#endif


#if 0


void Socket::Connect(const PL_CHAR *host, unsigned short port)
{
	if(m_socket==SOCKET_NULL || m_socket == INVALID_SOCKET)
		throw SocketException("Invalid socket while trying to connect");

	hostent *hp = gethostbyname(host);

	if(hp == NULL)
		throw(SocketException("Could not find host while trying to connect", GetError()));
		
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = *((unsigned long*)hp->h_addr);//inet_addr( IPAdress.c_str() );
	clientService.sin_port = htons( port );

	if ( connect( m_socket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR) 
	{
		m_socket = INVALID_SOCKET;
		throw SocketException("Failed to connect", GetError(), host, port);
	}
	//	delete hp?
	return;
}
#endif


void Socket::Connect(sockaddr_in *host, unsigned short port)
{
	if(m_socket==SOCKET_NULL || m_socket == INVALID_SOCKET)
		throw SocketException(_T("Invalid socket while trying to connect"));

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = host->sin_addr.s_addr;
	clientService.sin_port = htons(port);

	if ( connect(m_socket, (SOCKADDR*) &clientService, sizeof(clientService) ) == SOCKET_ERROR )
	{
		m_socket = INVALID_SOCKET;
		throw SocketException(_T("Failed to connect"), GetError());
	}
	return;
}


void Socket::DisconnectSend()
{
	if(m_socket==SOCKET_NULL) return;

#ifdef _WIN32
	if(shutdown(m_socket, SD_SEND) == SOCKET_ERROR)
		throw SocketException(_T("Failed to shut down send socket"), GetError());
#else
	if(shutdown(m_socket, SHUT_WR) == -1 )
		throw SocketException(_T("Failed to shut down send socket"), GetError());
#endif

}

void Socket::DisconnectReceive()
{
	if(m_socket==SOCKET_NULL) return;

#ifdef _WIN32
	if(shutdown(m_socket, SD_RECEIVE)==SOCKET_ERROR)
		throw SocketException(_T("Failed to shut down receive socket"), GetError());
#else
	if(shutdown(m_socket, SHUT_RD)== -1 )
		throw SocketException(_T("Failed to shut down receive socket"), GetError());
#endif

}


void Socket::Disconnect()
{
	if(m_socket==SOCKET_NULL) return;
//	WSAWaitForMultipleEvents(1, (const WSAEVENT*)&evt, 0, 3000, 1);
#if defined(_WIN32)
	if(shutdown(m_socket, SD_BOTH)==SOCKET_ERROR)
		throw SocketException(_T("Failed to shut down socket"), GetError());
#elif defined(_OSX)
	if(shutdown(m_socket, SHUT_RDWR)== -1 )
	{
		int err = GetError();
		if(err != 57)
			throw SocketException(_T("Failed to close socket"), GetError());
	}
#else
	if(shutdown(m_socket, SHUT_RDWR)== -1 )
		throw SocketException(_T("Failed to close socket"), GetError());

#endif
}


//******************************************
//		server stuff		
//******************************************

void Socket::Bind(unsigned short port)
{
    if(m_socketType == UNIX) throw SocketException(_T("Invalid socket type for Bind"));

	struct sockaddr_in service;
	memset(&service, 0, sizeof(sockaddr_in));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr =  INADDR_ANY;
	service.sin_port = htons( port );
    
	// bind the socket to the port
	if ( bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) 
	{
		throw SocketException(_T("Bind to port failed."), GetError(), _T("inaddr_any"), port);
	}
}

#ifndef _WIN32
void Socket::Bind(const char *path)
{
	if(m_socketType != UNIX) throw SocketException("Invalid socket tyoe for bind");
	if(path==NULL) throw SocketException(_T("Invalid path for unix socket"));

	struct sockaddr_un local;
	int len;
	local.sun_family = AF_UNIX;  	
	strcpy(local.sun_path, path);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if( bind(m_socket, (struct sockaddr *)&local, len) == SOCKET_ERROR )
	{
		throw SocketException(_T("Bind to file failed."), GetError());
	}
}
#endif

size_t Socket::GetDatagram(char *buf, int len)
{
	socklen_t l = sizeof(struct sockaddr_in);
	size_t ret = recvfrom(m_socket, buf, len, 0, m_from, &l );
	if(ret == (size_t)-1) throw SocketException(_T("Error getting datagram"), errno);
	buf[ret] = '\0';

	return ret;

}

void Socket::Listen(int port, bool nonBlocking)
{
    if(m_socketType == UNIX) throw SocketException(_T("Invalid Socket type for Listen"));
	if(nonBlocking)
	{
#ifdef _WIN32
		u_long arg = 1;
		ioctlsocket(m_socket, FIONBIO, &arg);
#else
		int x;
		x=fcntl(m_socket,F_GETFL,0);
		fcntl(m_socket,F_SETFL,x | O_NONBLOCK);
#endif
	}

	this->Bind(port);

	// listen
	if ( listen( m_socket, MAX_NUM_CONNECTIONS ) == SOCKET_ERROR )
	{
		throw SocketException(_T("Listen failed"), GetError(), _T("inaddr_any"), port);
	}

	return;
}

#ifndef _WIN32
void Socket::Listen(const char *path)
{
	if(m_socketType != UNIX) throw SocketException("Invalid socket type while trying to listen");

	this->Bind(path);

	if( listen( m_socket, MAX_NUM_CONNECTIONS) == SOCKET_ERROR )
	{
		throw SocketException(_T("Listen Failed"), GetError());
	}
}
#endif

bool Socket::Accept(SOCKET &new_sock) 
{
	fd_set fd;
	struct timeval tv;
	int retval;

	FD_ZERO(&fd);
	FD_SET(m_socket, &fd);

	tv.tv_sec  = 1;
	tv.tv_usec = 0;
	retval = select(m_socket+1, &fd, NULL, NULL, &tv);

	if(retval != 1) return false;
	new_sock = accept(m_socket, 0,0);

	if (new_sock == INVALID_SOCKET) 
	{
		if(GetError() == SOCKET_WOULDBLOCK) return false;
		throw SocketException(_T("Could not accept") , GetError());
	}

	return true;
}


/******************************************
                  SEND
******************************************/


void Socket::Send(std::vector<char> &data)
{
    Send((void*)data.data(), (int)data.size());
}

void Socket::Send(PL_CHAR c)
{
	PL_CHAR data[1];
	data[0] = c;
	Send(data, sizeof(PL_CHAR));
}

void Socket::Send(void *data, int len)
{
	OSTREAM ss;
	int bytesSent = 0;

	if(m_socket==SOCKET_NULL)
		throw(SocketException(_T("Error trying to send data while disconnected"), GetError()));

	//bytesSent = send( m_socket, (char*)data, len, MSG_NOSIGNAL );
	bytesSent = send( m_socket, (char*)data, len, 0 );

	if (bytesSent == SOCKET_ERROR)
	{
		int err = GetError();
		if(err==SOCKET_WOULDBLOCK)
			return; 

		ss << "Error sending data";
		throw SocketException(ss.str(), err);
	}

	if(bytesSent != len)
	{
		ss << _T("Error not all data was sent: ") << bytesSent << " of " << len;
		throw SocketException(ss.str(), GetError());
	}
}


/*
bool Socket::SendUrl(const char *url, string &header, string &data)
{
	Connect();
	Send(url);
	return ReceiveHttp(header, data);
}*/


bool Socket::PutFile(const PL_CHAR *filename, long offset)
{
	pl_data buffer[4096];
	bool b;
	size_t bytesRead;

	peel::File pf(filename, STRLEN(filename));
	pf.open("rb");
	if(!pf.okay())
		return 0;

	if(offset != 0)
		pf.seek(offset);

	pf.read(buffer, 4096, &bytesRead);
	b = (bytesRead != 0);
	
	while(b)
	{
		if(bytesRead==0) break;
		Send(buffer, (int)bytesRead);  // will throw
		pf.read(buffer, 4096, &bytesRead);
	}

	pf.close();

	return true;

}





/******************************************
                 Receive
******************************************/


void Socket::Receive(void *buf, int len, int *recd)
{
	int bytesRecv = 0;
	bytesRecv = recv( m_socket, (char*) buf, len, 0 );

	if ( bytesRecv == 0 )
		throw SocketException(_T("Connection Closed"));

	if(bytesRecv == SOCKET_ERROR)
	{
		int err=GetError();
		if(err == SOCKET_WOULDBLOCK) return;
		throw SocketException(_T("Error Reading"), err);
	}

	if(recd != NULL) *recd = bytesRecv;
}

void Socket::Receive(void *buf, int len)
{
	int count = 0;
	int recd = 0;

	while(count < len)
	{
		Receive((char*)buf + count, len-count, &recd);
		count += recd;
	}
}


// Receives a raw file that is waiting on the pipe (headers have already been parsed

void Socket::ReceiveRaw(STRING filename, int len)
{
	string data = "";
	OSTREAM ss;
	size_t  bytesRecv = 0;
	time_t  startTime = time(NULL);
	size_t  written;
	size_t  total=0;
	pl_data buf[255];

	peel::File pf(filename.c_str(), filename.length()); 
	pf.open("wb");
	if(!pf.okay())
		throw SocketException(_T("Could not create file"),0);
	

#ifdef _WIN32
	u_long arg = 1;
	ioctlsocket(m_socket, FIONBIO, &arg);
#else
	int x;
	x=fcntl(m_socket,F_GETFL,0);
	fcntl(m_socket,F_SETFL,x | O_NONBLOCK);
#endif

	while(total<len)
	{
		if(difftime(time(NULL),startTime) > TIMEOUT)
			throw SocketException(_T("Timeout"),0);

		bytesRecv = recv( m_socket, (char*)buf, 255, 0 );
			
		if(bytesRecv == SOCKET_ERROR)
		{
			int err = GetError();
			if(err == SOCKET_WOULDBLOCK)
				continue;

			pf.close();
			throw SocketException(_T("Receive File Error"), err);
		}
#ifdef _WIN32
		if ( bytesRecv == 0 || bytesRecv == SOCKET_CONNRESET ) 
#else
		if ( bytesRecv == 0)
#endif
		{
			pf.close();
			throw SocketException(_T("Connection Closed"),0);
		}
		
		 
		if( bytesRecv > 0 )
		{
			pf.write(&buf[0], bytesRecv, &written);
			if(written == bytesRecv)
			{
				pf.close();
				ss.str(_T(""));
				ss << _T("Could not write to file ") << GetError();
				throw SocketException(ss.str(),0);
			}
			total+= written;
		}		
	}

 	if(total!=len)
	{
		pf.close();
		ss.str(_T(""));
		ss << total << _T(" of ") << len << _T(" recieved.");
		throw SocketException(ss.str(),0);		
	}

	pf.close();
	return;
}




bool Socket::Receive(std::vector<char> &data)
{
    char    c[RECEIVE_BUFF_SIZE];
	int     bytesRecv = 0;
	time_t  startTime = time(NULL);

	while(bytesRecv >= 0 && difftime(time(NULL),startTime) < TIMEOUT )
	{
        Receive((void*)c, (RECEIVE_BUFF_SIZE) -1, &bytesRecv);

		if ( bytesRecv > 0 )
		{
            std::copy( c, c+bytesRecv, data.begin() );
		}
	}

	if(data.size()>0)
		return true;
	
	return false;
}


bool Socket::ReceiveToToken(void *data, int len, const char token, int *bytesRead)
{

	char    c;
	char    *ptr = (char*)data;
	int     bytesRecv = 0; 
	if(len == 0) throw SocketException(_T("Invalid data length while receiving token"));

	while(bytesRecv >= 0)// && difftime(time(NULL),startTime) < TIMEOUT )
	{
		Receive((void*) &c, 1, &bytesRecv);

		if ( bytesRecv > 0 )
		{
			*ptr = c;
			ptr++;
			if(ptr - (char*)data == len) throw SocketException(_T("Overflow while receiving token"));
			if ( c == token )
			{
				*bytesRead = (int)(ptr - (char*)data);
				return true;
			}
		}
	}
	
	return false;
}


/*
bool Socket::ReceiveToToken(STRING &data, STRING token, bool *found)
{
	OSTREAM ss;
	PL_CHAR   c[256];
	int     bytesRecv = 0;
	time_t  startTime = time(NULL);

    data = _T("");
	if(found != NULL) *found = false;

#ifdef _DEBUG
	const PL_CHAR *cstr;
#endif

	while(bytesRecv >= 0 && difftime(time(NULL),startTime) < TIMEOUT )
	{
		//TODO read more
		bytesRecv = recv( m_socket, (char*)c, sizeof(PL_CHAR), 0 );
		c[bytesRecv]=_T('\0');
			
		if ( bytesRecv < 0 && errno == SOCKET_WOULDBLOCK ) break;
#ifdef _WIN32	
		if ( bytesRecv == 0 || bytesRecv == SOCKET_CONNRESET ) break;
#else	
		if ( bytesRecv == 0 ) break;
#endif
		if ( bytesRecv > 0 )
		{
			data += c;
#ifdef _DEBUG
			cstr = data.c_str();
#endif
		}
		if(data.size()> token.size())
		{
			if(data.substr(data.size()-token.size(),token.size())==token)
			{
				if(found != NULL) *found = true;
				break;
			}
		}
	}

	if(data.size()>0)
		return true;
	
	return false;
}
*/


