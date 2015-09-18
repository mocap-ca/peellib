#ifndef ___SOCKET_H___
#define ___SOCKET_H___

#include "chartype.h"

#include "peel/File.h"
#include <string>
#include <iostream>
#include <time.h>
#include <sstream>

#include "peel/SocketDefines.h"
#include "peel/SocketAddress.h"

using namespace std;


namespace peel
{

typedef enum eSocketType
{
	INVALID = 0,
	TCP,
	UDP,
	UNIX
} TSocketType;
	
	
// constants
const int RECEIVE_BUFF_SIZE = 1024;
const int MAX_NUM_CONNECTIONS = 10;

#ifdef _WIN32
const double TIMEOUT =30.0;
#else
#define TIMEOUT 30
#endif



// TODO: change bool returns to void and create throw events instead of false

//! A class wrapper for functions that act on a SOCKET.
/*! This class provides access to many socket fuctions both for creating sockets
    that listen like a server, or connect to a remote server and send data.
*/

class Socket 
{

public:
	// Constructors
	Socket();              //!< Create a tcp socket object that is not connected
	Socket(TSocketType t); //!< Create a socket of specified type that is not connected
	Socket(SOCKET s);      //!< Create a socket object using an existing socket
	~Socket();        //!< Will call DestroySocket() and cleanup

	//! operator cast to the m_socket membera - pretty hacky.
	operator const SOCKET&() { return m_socket; }

	// Socket Creation/Destruction
	void CreateSocket(int type);    //!< Calls socket() - throws SocketException
	void DestroySocket();           //!< Calls closesocket() - throws SocketException


	/*
		Information
	*/

	//! Returns m_socketType
	TSocketType GetType();

	//! Returns platform error - errno or WSAGetLastError()
	static int GetError();     

	//! Calls gethostname()
	static STRING   GetComputerName();

	//! Look up a host and return its hostent->h_addr_list info
	u_long   LookupAddress (const PL_CHAR* pcHost);

	//! Return the local name that m_socket is bound to
	STRING   GetName();

	//! Get the name of the host on the other end of the line
	STRING   GetPeer();

	//! Modify the flags on m_socket to make it blocking/non-blocking	
	void     SetBlocking(bool blocking);

	/*
		UDP
	*/

	//! Set broadcast permission
	/*! Socket must be UDP */
	void     SetBroadcast(bool broadcast);

	//! Set broadcast destination - stored in m_sendDest
	/*! Socket must be UDP */
	void     SetDestination(const char*,  unsigned short port);

	//! Set broadcast destination - stored in m_sendDest
	/*! Socket must be UDP */
	void     SetDestination(struct sockaddr_in &in, unsigned short port);

	//! Send a datagram to the destination specified by SetDestination()
	/*! uses m_sendDest as desination 
	    Socket must be UDP
 	*/
	void     SendTo(const void * data, size_t len);

	/*
		Client
	*/

	// Init

	//! open a connection to the host/port
	void     Connect(const PL_CHAR *host, unsigned short port);

	//! open a connection to the host/port
	void     Connect(sockaddr_in *host, unsigned short port);

	void     Disconnect();         //!< shut down a connection
	void     DisconnectSend();     //!< shut down a connection for sending data
	void     DisconnectReceive();  //!< shut down a connection for reading data


	/*
		Server
	*/

	//! Bind the server to a port. 
	/*! Throws a SocketException if bind fails
	    This method is called by listen, so likely does not need to be called directly
	 */
	void     Bind(unsigned short port);

#ifndef _WIN32
	//! Bind to a unix socket (file)
	void     Bind(const char *path = NULL);
#endif

	size_t   GetDatagram(char *msg, size_t maxLen);

	
	//! Listen for a connection.
	/*! Set the socket options for blocking, calles Bind then Listen.
	    If blocking is false, function should return right away 
	    Throws a SocketException if Bind or Listen fails */
	void     Listen(int port, bool nonBlocking);   

#ifndef _WIN32
	//! Listen for a connection on a unix socket
	void  Listen(const char *path);
#endif

	//! Accept an incoming connection.  
	/*! Will poll the listening socket and wait for one second for an incoming connection.  The
	    reason for doing this rather than using a blocking socket is on unix, the accept will
		not unblock when the socket is destoryed, so putting this in a thread and checking the
		shutdown status is required.
	*/
	//! \retval true if a connection was made
	
	bool     Accept(SOCKET &s);  



	/*
		Send
	*/

	void     Send(const STRING & data);  //!< Puts data on the wire.
	void     Send(void *data, int len);  //!< Puts data on the wire
	void     Send(PL_CHAR c);            //!< Puts a single character on the wire

	//! Put a file directly on the socket
	/*! @param offset send data from offset value to end of file */
	bool     PutFile(const PL_CHAR *filename, long offset);



	/*
		Receive 
	*/

	//! Receive data
	/*! Will keep reading in data till TIMEOUT has been exceeded or connection is closed. */
	bool     Receive(STRING &data);

	//! Receive a specific length of data
	void     Receive(void *buf, int len, int *recd);

	//! Recieve a specific length of data, and keep going till its all read.
	void     Receive(void *buf, int len);

	//! Receive everything up to a token
	/*! Will keep reading in data till TIMEOUT has been exceed or the token is read */
	//bool     ReceiveToToken(STRING &data, const PL_CHAR token, bool *foundToken = NULL);

	//! Receive everything up to a token
	/*! Will keep reading in data till TIMEOUT has been exceed or the token is read */
	bool     ReceiveToToken(void *data, int len, const char token, int *bytesRead = NULL);

	//! Receive a specific number of bytes. 
	bool     ReceiveByteCount(void *data, int bytes);

	//! Get binary data and save it as a file
	void     ReceiveRaw(STRING filename, int len);

//protected:

	//! The platform socket object
	SOCKET       m_socket;

	//! The type of socket, TCP or UDP
	TSocketType  m_socketType;

	//! Used by SetDestination and Send to for sending UDP Packets
	SocketAddress m_sendDest;

	//! Used by GetDatagram to store remote information
	SocketAddress m_from;

};


#ifdef _WIN32
extern WSADATA gWsaData;
bool SocketsInit();
bool SocketsCleanup();
#endif

} // namespace peel

#endif
