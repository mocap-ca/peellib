#include "peel/Thread.h"
#include "peel/Lock.h"
#include <stdio.h>


class Tester : public peel::Thread
{
	public:
	Tester() : m_condition(&m_conditionMutex) {}

	void operator()()
	{
		while(1)
		{
			printf("Waiting");

			int milliseconds = 5000;
                        struct timespec t;
                        t.tv_sec = milliseconds / 1000;
                        t.tv_nsec = milliseconds % 1000 * 1000000;

			printf("Secs: %i\nNano: %i\n", t.tv_sec, t.tv_nsec);

			m_condition.wait(5000);
		}
	}

	peel::ConditionMutex m_conditionMutex;
	peel::Condition      m_condition;

};
	


int main(int argc, char **argv)
{
	Tester t;
	t.start();

	char *buf = (char*)malloc(32);
	size_t len = 32;
	while(1)
	{
		getline(&buf, &len, stdin);
		t.m_condition.notify();
		printf("coop");
	}


}

