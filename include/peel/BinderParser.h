#ifndef ___PEELLIB_BINDERPARSER_DATA___
#define ___PEELLIB_BINDERPARSER_DATA___



#include "peel/BinderData.h"
#include "chartype.h"
#include <vector>

namespace peel
{

//! Parses a file containing BinderData blocks of data
/*! After creating an instance of this class, each othe BinderData objects need to 
 *  be registered by called registerParser().
 *  parser() is called to parse the data in the file.  First the header is read for
 *  some basic header data including the version number, flags and number of items
 *  in the parser (parserHeader()). Then the data is read out of the file
 *  (parseIndex()).  For each data block, the corresponding BinderParser object is 
 *  found (by size_t id) and the parseBlock() method is called to import the data
 */
class BinderParser : public File
{
public:
	//! create the instance of the file
	BinderParser(const PL_CHAR *file, pl_uint32 v, pl_uint32 u) 
	: File(file) , m_items(0), m_version(v), m_unique(u), m_err(0)
	{};

	//! parse a file, calling BinderData::readBlock for each item
	/* calls parseHeader() then parserIndex() */
	bool parse();

	//! read the header and set value for m_items
	bool parseHeader();

	//! parse the index, parseHeader must have been called first
	bool parseIndex();

	//! call the virtual dump function rather than using the BinderData
	bool dumpIndex();

	//! dump the index somehow, e.g. display it
	/*! This method should be overridden with something useful */
	virtual void dump(pl_uint32 type, pl_uint32 offset) {}

	//! write the binder file to disk
	/* calls File:open("w") to open the file and BinderData::write(this) 
	 * to do the writing to disk */
	bool write();

	//! add a BinderData item to m_dataa
	/*! There should only be one BinderData for each size_it id which should be
	    be able to import all the data for that block and handle arrays of data,
	    etc within it's own internal structures */
	bool registerParser(BinderData *);

	//! close the file
	/*! Calls File::close() */
	bool close() { return File::close(); }

	//! finds the first BinderData parser by id to process a block of data
	BinderData * findParser(pl_uint32 id);

	//! list of parsers that can process data
	std::vector<BinderData *> m_data;

	//! header flag - true if file came from a 64 bit system
	bool      m_64bitptr;

	//! header field - number of items in the file, as defined by the header
	pl_uint32 m_items;

	//! header field - version number of this file
	pl_uint32 m_version;

	//! header field - unique data to indentify this type of file
	pl_uint32 m_unique;

	//! error code
	int m_err;

};

}




#endif
