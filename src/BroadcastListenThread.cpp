#include "peel/BroadcastListenThread.h"
#include "peel/SocketException.h"

#include <iomanip>


using namespace std;
using namespace peel;

BroadcastListenThread::BroadcastListenThread(unsigned short port)
: Socket(UDP), m_port(port) , m_status(INIT) {}


void BroadcastListenThread::operator ()()
{
	OSTREAM  ss;
	size_t   ret;
	char     buf[256];

	try { this->Bind(m_port); }
	catch(SocketException e) { return; }

	m_status=RUNNING;

	while(m_status==RUNNING)
	{
		// get  a connection - will try for one second, and returns true
		// if a connection was made
		try
		{
			ret = this->GetDatagram(buf, 255);
		}
		catch(SocketException e)
		{
			break;
		}

		buf[ret] = 0;

		processData(buf, ret, &m_from);  // virtual
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



