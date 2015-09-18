
#ifdef PROFILE
#define INITCLOCK()	InitClock()
#define STARTCLOCK(c)	StartClock(c)
#define STOPCLOCK(c)	StopClock(c)
#define PRINTCLOCKS	PrintClocks()
#else
#define INITCLOCK()
#define STARTCLOCK(c)
#define STOPCLOCK(c)
#define PRINTCLOCKS
#endif

namespace peel
{

bool InitClock();
void StartClock(int c);
void StopClock(int c);
void GetClock(int c, char *);
double GetTime(int c);

}

