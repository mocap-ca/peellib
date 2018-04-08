
#include "Windows.h"
#pragma comment(lib, "Winmm.lib")
#include <stdio.h>

#include <sstream>
namespace peel
{

// link with winmm.lib

#define MAX_CLOCKS 30

static __int64 freq = 1000;
static LARGE_INTEGER starts[MAX_CLOCKS];
//static DWORD starts[MAX_CLOCKS];
static __int64 duration[MAX_CLOCKS];
static __int64 max_duration;
static int counter[MAX_CLOCKS];
static int num_clocks;

bool InitClock()
{
	timeBeginPeriod(1);
	LARGE_INTEGER frequ;
	if (!QueryPerformanceFrequency(&frequ))
		return false;
	freq = frequ.QuadPart;

    return true;
}

void StartClock(int c)
{
//	starts[c] = GetTickCount();
//	starts[c] = timeGetTime();
	if (!QueryPerformanceCounter(&starts[c]))
		fprintf(stderr, "Error getting start time!\n");
}

void StopClock(int c)
{
	LARGE_INTEGER end;
//	DWORD end;
//	end = GetTickCount();
//	end = timeGetTime();
	if (!QueryPerformanceCounter(&end))
		fprintf(stderr, "Error getting end time!\n");
	counter[c]++;
//	duration[c] += end - starts[c];
	duration[c] += (end.QuadPart - starts[c].QuadPart);
	if (max_duration < duration[c])
		max_duration = duration[c];
	if (num_clocks < c)
		num_clocks = c;
}

void GetClock(int c, char *buf)
{
	std::ostringstream oss;

	oss << "Counter: " << c << "  " << \
		((double)duration[c] / (double)max_duration) * 100. << \
		(double)duration[c]/(double)freq << \
		((double)duration[c]/(double)counter[c])/(double)freq << \
		counter[c];
}

double GetTime(int c)
{
	return (double)duration[c] / (double)freq;
}

}