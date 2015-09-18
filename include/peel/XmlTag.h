#ifndef ____XMLTAG_H__
#define ____XMLTAG_H__

#include "peel/Xml.h"

namespace peel
{

	

// Tag Entity, e.g. <ANIMAL TYPE="DOG" BREED="POODLE"><PET>Bubbles</PET></ANIMAL>
// tagName = "ANIMAL"
// subTags = "<PET>Bubbles</PET>"
// tags: ("TYPE","DOG"), ("BREED","POODLE");

//! An structure to hold an xml tag entity and query its attribtes

class XmlTagEntity : XmlEntity
{
public:
	XmlTagEntity(long offset);
	~XmlTagEntity();
	inline classType getClassType() { return type_tag; }
	inline bool      isTag(){ return true; }

	long   parse();    //!< Parse the data
	long   parseOne(); //!< Parse a single entity
	long   parseSub(); //!< Parse sub entities
	bool   unParse();
	bool   parseTag(STRING tag);
	STRING getArgs();
	bool   getArg(STRING, STRING&);  //<! Get a string argument, returns default on error
	bool   getArg(STRING, int&);     //<! Get an integer argument, return default on error
	bool   getArg(STRING, unsigned int&);  //<! Get an integer argument, return default on error
	bool   getArg(STRING, long&);          //<! Get a long integer argument, return default on error
	bool   getArg(STRING, float&);         //<! Get a float argument, return default value on error
	bool   getArg(STRING, double&);        //<! Get a double argument, return default value on error
	bool   getArg(STRING, bool&);          //<! Get a book argument, either 1 or 0.  Return default on error
#ifdef _WIN32
	inline bool   getArg(STRING s, DWORD &di) { int i=0; bool b = getArg(s, i); di= i; return b; }
#endif
	XmlTagEntity*  findSubTag(STRING name);
	XmlEntity*     getSub(size_t pos);
	XmlTagEntity*  getSubTag(size_t pos);
	size_t         subTags();
	STRING         getName();
	bool           isName(STRING);
	STRING         getSubValue();
	XmlEntity*     getNext();
	XmlTagEntity*  getNextTag();



	void           reset() { m_pos = m_offset; }
	long           reParse();
	void           error(STRING);

	bool                m_closing;
	bool                m_selfClosing;
	bool                m_complete;
	bool                m_ready;
	bool                m_error;
	STRING              m_errorString; 

	vector <XmlEntity*> m_subTags;
	long                m_offset;

private:
	STRING              m_name;

	long                m_pos;

	size_t              m_next;
	size_t              m_inserted;


    STRING              m_tag;
	vector<strpair>     m_arguments;

};


} //namespace

#endif

