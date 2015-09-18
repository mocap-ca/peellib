#include "peel/c3dFile.h"
#include <stdio.h>

class c3dTest : public peel::c3dFile
{
public:

	virtual bool createGroup(string name) {};
	virtual bool createLocator(size_t i, string name, bool isPoint) {};
	virtual void setSizes(size_t pointSize, size_t channelSize) {};
	virtual bool addKey(size_t marker, float sample, float x, float y, float z) {};
	virtual void setRange(int start, int end) {};
	virtual void setRate(float) {};
	virtual bool addAnalogKey(size_t marker, float sample, float value) {};
	virtual bool frameDone(int time) {};
	virtual void allDone() {};
};

int main(int argc, char *argv[])
{
	c3dTest t;
	if(argc < 2)
	{
		fprintf(stderr,"Usage %s c3dFile.c3d\n", argv[0]);
		return 1;
	}
	if(!t.open(argv[1]))
	{
		fprintf(stderr,"Could not open file\n");
		return 2;
	}
	if(!t.parse(NULL, true,false))
	{
		fprintf(stderr, "Could not parse file\n");
		return 3;
	}
	printf("%f\n", t.m_frameRate);
	printf("%d\n", t.m_header.last_field - t.m_header.first_field);
	
	return 0;
}
