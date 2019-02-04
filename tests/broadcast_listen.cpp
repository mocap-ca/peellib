#include "peel/Socket.h"
#include "peel/SocketException.h"
#include "peel/BroadcastListenThread.h"

class BListen : public peel::BroadcastListenThread
{
public:
	BListen(int port) : peel::BroadcastListenThread(port)
	{}

	void processData(char *data, size_t len, peel::SocketAddress *from)
	{
		printf("%s", data);
		for(size_t i =0; i< len; i++)
			if(data[i] == 'x')
				m_status = peel::BroadcastListenThread::END;
	}
};

int main(int argc, char *argv[])
{
	//if(argc < 2)
	//{
	//	fprintf(stderr, "Usage %s port\n", argv[0]);
	//	exit(1);
	//}

	char port[] = "8889";
	char buf[255];

#ifdef _WIN32
	peel::SocketsInit();
#endif


	try
	{
		BListen l(atoi(port));
		l.start();
		l.join();
	}
	catch(peel::SocketException e)
	{
		printf("%s\n", e.str().c_str());
		exit(1);
	}

#ifdef _WIN32
	peel::SocketsCleanup();
#endif

	return 0;
}
	
