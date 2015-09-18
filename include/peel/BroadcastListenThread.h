#ifndef ___PEEL_BROADCAST_LISTEN_THREAD_H____
#define ___PEEL_BROADCAST_LISTEN_THREAD_H____

//#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>


#include "peel/Socket.h"
#include "peel/Signal.h"
#include "Thread.h"

namespace peel
{


//! Creates a thread that waits on an incomming broadcast message (UDP)
class BroadcastListenThread : public Thread, public Socket
{
public:
	//! Current status of the thread
	enum status { INIT, RUNNING, TERM, END };

	//! Specify the port to listen on
	BroadcastListenThread(unsigned short port);

	//! Stop the thread
	void stopListening() { m_status = TERM; }

	//! Returns true if the thread is listening for connections
	bool isListening()   { return m_status == RUNNING; }

	//! Returns true of the thread has ended
	bool ended()         { return m_status == END; }


	//! Subclass should implement this to handle the incoming data connection
	virtual void processData(char *data, size_t len, SocketAddress *from) = 0;

	//! Port to listen on
	unsigned short    m_port;

	//! Current status.
	status m_status;


private:
	//! worker function, calls receiveData to do the work
	void operator()();	

};


} // namespace

#endif

