#include "peel/Socket.h"
#include "peel/SocketException.h"
#include "peel/ListenThread.h"

class Listener : public peel::ListenThread
{
public:
	Listener(int port) : peel::ListenThread(port)
	{}

	void receiveData(Socket *s)
	{
		char buf[1024];
		char name[1024];
		float transform[6];
		int recd;
		while(1)
		{	
			try
			{
				s->Receive(buf, 1024, &recd);
				if(recd==0) break;
				buf[recd]=0;
				printf("%s", buf);
			}
			catch(peel::SocketException e)
			{
				printf("Error: %s\n", e.str().c_str());
				break;
			}
		}
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
		Listener l(atoi(port));
		l.start();
		printf("Listening on port: %s\n", port);
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

	printf("Clean finish\n");
	return 0;
}
	
