#include "peel/BinderParser.h"

#define STATIC_HEADER 0x2125
#define INDEX_WIDTH   (3*sizeof(pl_uint32))
#define FLAG_64BIT_PTR 1

#if defined(_DEBUG) && !defined(_WIN32)
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

using namespace peel;

bool BinderParser::registerParser(BinderData *item)
{
	m_data.push_back(item);
	return true;
}


BinderData* BinderParser::findParser(pl_uint32 id)
{
	for(std::vector<BinderData*>::iterator i = m_data.begin();
		i != m_data.end(); i++)
	{
		if(*i == NULL) continue;
		if((*i)->m_id == id) return *i;
	}
	return NULL;
}

bool BinderParser::parseHeader()
{
	pl_uint32 val   = 0;
	pl_uint32 flags = 0;

	// read static data
	if(!read4(val)) { m_err = 1; return false; }
	if(val != STATIC_HEADER) { m_err = 2; return false; }

	// read flags
	if(!read4(flags)) { m_err = 3; return false; }
	if(flags & FLAG_64BIT_PTR) { m_err = 4; m_64bitptr = true; }	

	// read version
	if(!read4(val)) { m_err = 5; return false; }
	if(m_version != (pl_uint32)-1 && val != m_version)
	{ m_err = 6; return false; }

	// read unique data
	if(!read4(val)) { m_err = 7; return false; }
	if(m_unique != (pl_uint32)-1 && val != m_unique)
	{ m_err = 8 ; return false; }

	// read the number of items
	if(!read4(m_items)) { m_err = 9; return false; }

	return true;
}


bool BinderParser::parse()
{
	m_64bitptr = false;

	// open the file for reading
	if(!open("r")) return false;

	if(!parseHeader()) return false;

	if(!parseIndex()) return false;

	File::close();

	return true;
}

bool BinderParser::dumpIndex()
{
	pl_uint32 *type, *offset;
	pl_sizet  len   = 0;

	// read the index
	pl_data* lines = (pl_data*)malloc(m_items * INDEX_WIDTH);
	read(lines, (pl_sizet)(m_items * INDEX_WIDTH), &len);
	if(len != m_items * INDEX_WIDTH) return false;

	// parse each item
	for(pl_uint32 i=0; i < m_items; i++)
	{		
		type   = (pl_uint32*)( lines + (i * INDEX_WIDTH)     );
		offset = (pl_uint32*)( lines + (i * INDEX_WIDTH) + 4 );
		dump(*type, *offset);
	}

	File::close();

	free(lines);

	return true;
}

bool BinderParser::parseIndex()
{
	pl_uint32 *type, *offset;
	pl_sizet  len   = 0;

	// read the index
	pl_data* lines = (pl_data*)malloc(m_items * INDEX_WIDTH);
	read(lines, (pl_sizet)(m_items * INDEX_WIDTH), &len);
	if(len != m_items * INDEX_WIDTH) return false;

	// parse each item
	for(pl_uint32 i=0; i < m_items; i++)
	{		
		type   = (pl_uint32*)( lines + (i * INDEX_WIDTH)     );
		offset = (pl_uint32*)( lines + (i * INDEX_WIDTH) + 4 );

		seek(*offset);

		BinderData *data = findParser(*type);
		if(data == NULL) continue;

		data->parseBlock(this, *offset, m_64bitptr);
	}

	free(lines);

	return true;
}


bool BinderParser::write()
{
	pl_sizet len;
	pl_uint32 flags = 0;
	pl_uint32 items = (pl_uint32)m_data.size();

#ifdef PL_64BIT
	flags = FLAG_64BIT_PTR;
#endif

	if(!open("w")) return false;

	// Write the header
	pl_uint32 st = STATIC_HEADER;
	File::write4(st);
	File::write4(flags);
	File::write4(m_version);
	File::write4(m_unique);
	File::write4(items);

	if(m_data.size() == 0)
		return close();

	// Write a stub for the index
	pl_sizet  indexSize = m_data.size() * 3 * 4;
	pl_data*  indexData = (pl_data*)malloc(indexSize);
	memset(indexData, 0, indexSize);
	File::write(indexData, indexSize, &len);
	if(len != indexSize) return false;

	// write the blocks
	pl_uint32 offset = 5*4 + (pl_uint32)indexSize;
	std::vector<pl_uint32> offsets;
	for(std::vector<BinderData*>::iterator i = m_data.begin();  i != m_data.end(); i++)
	{
		if(*i==NULL) continue;        // Skip null entries
		offsets.push_back(offset);    // Save the offset
		// Get the node to write the data
#if defined(_DEBUG) && ! defined(_WIN32)
		size_t pos1 = this->getPosition();
		size_t val  = (*i)->write(this);
		size_t pos2 = this->getPosition();
		if((pos2 - pos1) != val)
		{
			// using lu format spec becuase of osx compiler warning
#ifdef __LP64__
			printf("DEBUG ERROR: BinderParser::write - %lu - %lu != %lu\n", pos2, pos1, val);
#else
			printf("DEBUG ERROR: BinderParser::write - %u - %u != %u\n", pos2, pos1, val);
#endif
			kill(getpid(), SIGSTOP);
		}
		offset += val;
#else
		offset += (*i)->write(this);  
#endif

	}

	// Go back and write the index
	seek(5 * sizeof(pl_uint32));

	pl_uint32 zero = 0;

	for(pl_sizet i=0; i<m_data.size(); i++)
	{
		BinderData *item = m_data[i];
		if(item==NULL) continue;

		File::write4(item->m_id);
		File::write4(offsets[i]);
		File::write4(zero);
	}

	close();

	return true;
}
