#include "peel/Socket.h"
#include "peel/SocketException.h"

int main(int argc, char *argv[])
{
	//if(argc < 4)
	//{
	//	fprintf(stderr, "Usage %s ip port string\n", argv[0]);
	//	exit(1);
	//}

	//unsigned short port = atoi(argv[0]);
	char ip[]   = "255.255.255.255";
	char port[] = "4493";
	char msg[]  = "peelfarm localhost 4494 ";

	try
	{

		peel::Socket sock(peel::UDP);
		sock.SetBroadcast(true);
		sock.SetDestination(ip, atoi(port));
		sock.SendTo(msg, strlen(msg));
		sock.DestroySocket();
	}
	catch(peel::SocketException e)
	{
		printf("%s\n", e.str().c_str());
		exit(1);
	}

	return 0;
}
	
