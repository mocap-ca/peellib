#include "peel/Thread.h"
#include "peel/Lock.h"
#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#endif


peel::LockMutex *mutex;

class Tester : public peel::Thread
{
	public:
	Tester(size_t t) : m_time(t) {};

	void operator()()
	{
		for(int i=0; i < 5; i++)
		{
			peel::Lock l(mutex);
			printf("Waiting\n");
			sleep(m_time);
		}
	}
	size_t m_time;

};
	


int main(int argc, char **argv)
{
	mutex = new peel::LockMutex;

	Tester t1(3);
	Tester t2(4);
	printf("Start1\n");
	t1.start();
	printf("Start2\n");
	t2.start();


	t1.join();
	t2.join();

}

