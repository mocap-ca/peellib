#include <errno.h>
#include "peel/UnixUtil.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace PU
{



std::string unixError(int err)
{
	return string(strerror(err));
}

bool DirectoryExists(std::string dir)
{
	struct stat st;
	if(stat(dir.c_str(), &st) != 0) return false;
	return S_ISDIR(st.st_mode);
}

bool FileExists(std::string file)
{
	FILE *fp = fopen(file.c_str(), "r");
	if(fp==NULL)
		return false;

	fclose(fp);
	return true;
}

bool FileDelete(std::string file)
{
	return unlink(file.c_str())==0;
}



} // namespace
