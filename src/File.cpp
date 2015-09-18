#include "peel/File.h"



using namespace peel;

File::File()
: m_fp(NULL)
, m_path(NULL)
{}

File::File(const PL_CHAR *path) : m_fp(NULL), m_path(NULL)
{
	pl_sizet len = STRLEN(path);
	m_path = (PL_CHAR*) malloc((len + 1) * sizeof(PL_CHAR) );
	memcpy(m_path, path, len * sizeof(PL_CHAR));
	m_path[len]='\0';
}


File::File(PL_CHAR *path, pl_sizet len) : m_fp(NULL), m_path(NULL)
{
	m_path = (PL_CHAR*) malloc((len+ 1) * sizeof(PL_CHAR) );
	memcpy(m_path, path, len * sizeof(PL_CHAR));
	m_path[len]='\0';
}

File::File(const PL_CHAR *path, pl_sizet len) : m_fp(NULL), m_path(NULL)
{
	m_path = (PL_CHAR*) malloc((len+ 1) * sizeof(PL_CHAR));
	memcpy(m_path, path, len * sizeof(PL_CHAR));
	m_path[len]='\0';
}

File::File(STRING path) : m_fp(NULL), m_path(NULL)
{
	pl_sizet len = path.length();
	m_path = (PL_CHAR*) malloc((len + 1) * sizeof(PL_CHAR) );
	memcpy(m_path, path.c_str(), len * sizeof(PL_CHAR));
	m_path[len] = '\0';
}


File::~File()
{
	if(m_path != NULL) free(m_path);
	if(m_fp   != NULL) this->close();
}

bool File::okay()
{
	if(m_path==NULL || m_fp==NULL) return false;
	return true;
}



bool File::open(const char *mode)
{
	if(m_path==NULL) return false;
#ifdef _WIN32
	DWORD flags = 0;
	//DWORD flags2 = OPEN_EXISTING;
	DWORD flags2 = 0;
	if(mode[0]=='r')
	{
		flags  |= GENERIC_READ;
		flags2 |= OPEN_EXISTING;

		if(mode[1]=='+')
			flags |= GENERIC_WRITE;
	}
	else
	if(mode[0]=='w')
	{
		flags |= GENERIC_WRITE;
		if(mode[1]=='+')
		{
			flags |= GENERIC_READ;
		}
		flags2 = CREATE_ALWAYS;
	}
	
	m_fp = CreateFile(m_path, flags, FILE_SHARE_READ, NULL, flags2, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_fp == INVALID_HANDLE_VALUE)
	{
		m_fp = NULL;
		return false;
	}
	return true;
#else
	m_fp = fopen(m_path, mode);
	return m_fp != NULL;
#endif
}

void File::read(pl_data *buffer, pl_sizet size, pl_sizet *read)
{
#ifdef _WIN32
	DWORD r;
	ReadFile(m_fp, buffer, size, &r, NULL);
	if(read != NULL)
	{
		if(r<0) *read = 0;
		else *read = r;
	}
#else
	if(read != NULL)
		*read = fread(buffer, 1, size, m_fp);
	else
		fread(buffer, 1, size, m_fp);
#endif
}


bool File::read8(pl_data *data)
{
	pl_sizet rd;
	read(data, 8, &rd);
	if(rd != 8) return false;
	return true;
}

bool File::read8(const pl_uint64 &data)
{
	pl_sizet rd;
	read((pl_data*)&data, 8, &rd);
	if(rd != 8) return false;
	return true;
}

bool File::read4(pl_data *data)
{
	pl_sizet rd;
	read(data, 4, &rd);
	if(rd != 4) return false;
	return true;
}

bool File::read4(const pl_uint32 &val)
{
	pl_sizet rd;
	read((pl_data*)&val, 4, &rd);
	if(rd != 4) return false;
	return true;
}

bool File::read2(pl_data *data)
{
	pl_sizet rd;
	read(data, 2, &rd);
	if(rd != 2) return false;
	return true;
}

bool File::read1(pl_data *c)
{
	pl_sizet rd;
	read(c, 1, &rd);
	if(rd != 1) return false;
	return true;
}

bool File::read(STRING &val)
{
	pl_sizet rd;
	pl_uint32 len;
	read4((pl_data*)&len);
	if(len == 0)
	{
		val = _T("");
		return true;
	}

	PL_CHAR *dat = (PL_CHAR*) malloc((len + 1) * sizeof(PL_CHAR));
	if(dat == NULL) return false;

	read((pl_data*)dat, len * sizeof(PL_CHAR), &rd);
	if(rd != len * sizeof(PL_CHAR)) return false;

	dat[len] = 0;

	val = dat;

	free(dat);

	return true;
}


	


void File::write(pl_data *buffer, pl_sizet size, pl_sizet *written)
{
#ifdef _WIN32
	DWORD r;
	WriteFile(m_fp, buffer, size, &r, NULL);
	if(written != NULL)
	{
		if(r<0) *written = 0;
		else *written = r;
	}
#else
	if(written != NULL)
		*written = fwrite(buffer, 1, size, m_fp);
	else
		fwrite(buffer, size, size, m_fp);

#endif
}

bool File::write8(pl_data *val)
{
	pl_sizet l;
	write(val, 8, &l);
	if(l != 8) return false;
	return true;
}

bool File::write8(const pl_uint64 &val)
{
        pl_sizet l;
        write((pl_data*)&val, 8, &l);
        if(l != 8) return false;
        return true;
}

bool File::write4(pl_data *val)
{
	pl_sizet l;
	write(val, 4, &l);
	if(l != 4) return false;
	return true;
}

bool File::write4(const pl_uint32 &val)
{
	pl_sizet l;
	write((pl_data*)&val, 4, &l);
	if(l != 4) return false;
	return true;
}

bool File::write2(pl_data *val)
{
	pl_sizet l;
	write(val, 2, &l);
	if(l != 2) return false;
	return true;
}
bool File::write1(pl_data *val)
{
	pl_sizet l;
	write(val, 1, &l);
	if(l != 1) return false;
	return true;
}

template <typename T> bool File::write(T val)
{
	pl_sizet l;
	write(val, sizeof(T), &l);
	if(l != sizeof(T)) return false;
	return true;
}

#ifdef _WIN32	
bool File::seek(pl_uint64 offset)
{
	LONG low;
	LONG high;

	char *ptr = (char*)&offset;

	memcpy( &low, ptr, 4);
	memcpy( &high, ptr + 4, 4);
	SetFilePointer(m_fp, low, &high, FILE_BEGIN);
	return true;
}
#else
// NB: long varies sizes between systems
bool File::seek(long offset)
{
	fseek(m_fp, offset, SEEK_SET);
	return true;
}
#endif

long File::getPosition()
{
#ifdef _WIN32
	return SetFilePointer(m_fp, 0, NULL, FILE_CURRENT);
#else
	return ftell(m_fp);
#endif
}


bool File::close()
{
#ifdef _WIN32
	CloseHandle(m_fp);
#else
	fclose(m_fp);
#endif
	m_fp = NULL;
	return true;
}




long File::fileSize(const PL_CHAR *file)
{
#ifdef _WIN32

	HANDLE f = CreateFile(file, FILE_GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(f == INVALID_HANDLE_VALUE)
                return 0;


	DWORD size_high, size_low;
	size_low = GetFileSize(f, &size_high);

	if(size_low ==0 && size_high==0) return 0;
	if(size_low==INVALID_FILE_SIZE) return 0;
	if(size_high>0) return 0; // too big
	CloseHandle(f);
	
	return size_low;

#else
	struct stat sdata;
	stat(file, &sdata);
	return (long) sdata.st_size;
#endif
}

 
