#ifndef ___PEEL_LISTENTHREAD_H____
#define ___PEEL_LISTENTHREAD_H____

//#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>


#include "peel/Socket.h"
#include "peel/Signal.h"
#include "Thread.h"

namespace peel
{


//! Creates a thread that waits on an incomming connection.  
/*! Uses a Socket object for lisening and subclasses Thread to create the thread.
    Member operator()() will call Listen then Accept connections 
    Accept should wait 1 second between attempts as a workaround for unix sockets
    blocking on Accept, but not returning when the socket they are accepting for
    is destroyed, such as when the app is terminating.
 */
class ListenThread : public Thread, public Socket
{
public:
	//! Current status of the thread
	enum status { INIT, RUNNING, TERM, END };

	//! Specify the port to listen on
	ListenThread(int port);

	//! Stop the thread
	void stopListening() { m_status = TERM; }

	//! Returns true if the thread is listening for connections
	bool isListening()   { return m_status == RUNNING; }

	//! Returns true of the thread has ended
	bool ended()         { return m_status == END; }


	//! Subclass should implement this to handle the incoming data connection
	virtual bool receiveData(Socket *s) = 0;

	//! Port to listen on
	int    m_port;

	//! Current status.
	status m_status;


private:
	//! worker function, calls receiveData to do the work
	void operator()();	

};


} // namespace

#endif

