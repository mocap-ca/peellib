#include "peel/ListenThread.h"
#include "peel/SocketException.h"

#include <iomanip>


using namespace std;
using namespace peel;

ListenThread::ListenThread(int port)
: m_port(port) , m_status(INIT) {}


void ListenThread::operator ()()
{
	OSTREAM  ss;
	SOCKET   sok;

	try { this->Listen(m_port, false); }
	catch(SocketException e) { return; }

	m_status=RUNNING;

	while(m_status==RUNNING)
	{
		// get  a connection - will try for one second, and returns true
		// if a connection was made
		try
		{
			if(!this->Accept(sok)) continue;
		}
		catch(SocketException e)
		{
			break;
		}

		// Create socket object for the incomming data
		Socket *ps = new Socket(sok);  

		// Pass the socket on to the virtual method - NB: virtual method should delete ps
		receiveData(ps);  // virtual
	}

	// Clean up
	try
	{
		this->Disconnect();
	}
	catch(SocketException e)
	{}

	m_status=END;
	return;
}



