#include <stdio.h>
#include "peel/chartype.h"

int error(const char *msg)
{
	fprintf(stderr, "%s",  msg);
	return 1;
}

int main(int argc, char *argv[])
{
#ifdef PL_64BIT
	if(sizeof(pl_sizet)  != 8) return error("pl_sizet (64) fails");
#endif
#ifdef PL_32BIT
	if(sizeof(pl_sizet)  != 4) return error("pl_sizet (64) fails");
#endif
	if(sizeof(pl_int64)  != 8)  return error("pl_int64 fails");
	if(sizeof(pl_uint64) != 8)  return error("pl_uint64 fails");
	if(sizeof(pl_uint32) != 4)  return error("pl_uint32 fails");
	if(sizeof(pl_int32) != 4)  return error("pl_int32 fails");
	if(sizeof(pl_uint16) != 2)  return error("pl_uint16 fails");
	if(sizeof(pl_int16) != 2)  return error("pl_int16 fails");
	if(sizeof(pl_byte) != 1)  return error("pl_byte fails");

	pl_int64   i64 = -1;
	pl_uint64 ui64 = -1;
	pl_int32   i32 = -1;
	pl_uint32 ui32 = -1;
	pl_int16   i16 = -1;
	pl_uint16 ui16 = -1;
	pl_byte    c   = -1;

	if(i64  > 0) return error("i64 sign fails");
	if(ui64 < 0) return error("ui64 sign fails");
	if(i32  > 0) return error("i32 sign fails");
	if(ui32 < 0) return error("ui32 sign fails");
	if(i16  > 0) return error("i16 sign fails");
	if(ui16 < 0) return error("ui16 sign fails");
	if(c    < 0) return error("byte sign fails");
	
	return 0;

};

