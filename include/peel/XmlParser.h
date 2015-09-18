#ifndef ____XMLPARSER_H__
#define ____XMLPARSER_H__

#include "peel/Xml.h"
#include "peel/XmlTag.h"

#include <stdio.h>
#include <vector>
#include <string>

using namespace std;


namespace peel
{

//! Parses an xml stream, either FILE or string* and creates a XmlTagEntity tree
class XmlParser
{
public:
	XmlParser(FILE*);
	XmlParser(STRING*);
	
	XmlEntity* getTag(long &offset);
	//bool parse(XmlTagEntity*);
	void error(STRING);

	FILE*  m_fp;
	const STRING *m_buffer;

	long   m_position;
	bool   m_useFile;
	bool   m_error;
	STRING m_errorString;

	static XmlParser* m_instance;
	static void createInstance(FILE*);
	static void createInstance(STRING*);
	static XmlParser* getInstance() { return XmlParser::m_instance; };
};


} //namespace


#endif

