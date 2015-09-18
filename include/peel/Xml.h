#ifndef ___Xml_H___
#define ___Xml_H___

#ifdef _DEBUG
#ifdef _WIN32
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "peel/Util.h"
#include "peel/chartype.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

using namespace std;

#include "peel/profile.h"

namespace peel
{

class Xml;
class XmlEntity;
class XmlTagEntity;
class XmlValueEntity;
class XmlAttribute;


typedef pair<STRING, STRING> strpair;


//! A simple XML Parser
class Xml
{
public:
	Xml()
		: m_rootEntity(NULL)
		, m_data(NULL)
	{}
	~Xml();
	bool Parse();

	XmlTagEntity *m_rootEntity;

	bool         m_useFile;
	unsigned int m_filePos;
	FILE*        m_fp;
	
	const static size_t npos = (size_t)-1;

private:
	const STRING *m_data;

};



//! Entity Base class. 

class XmlEntity
{
public:
	enum classType { type_tag, type_value, type_base };

	                  XmlEntity() {};
	virtual           ~XmlEntity() {};
	virtual classType getClassType() { return type_base; };
	virtual bool      isTag() { return false; };

	bool error;
};

//! Value entity, e.g. string value: "Bubbles"
class XmlValueEntity : public XmlEntity
{
public:
	inline XmlValueEntity() { m_value = _T(""); }
	inline XmlValueEntity(STRING value) { m_value = value; }
	inline bool      isTag() { return false; }
	inline classType getClassType() { return type_value; }

	STRING m_value;
};


//! Attribute XML Entitiy, with name=value
class XmlAttribute : public XmlEntity
{
public:
	XmlAttribute() {m_name = _T(""); m_value=_T(""); }
	XmlAttribute(STRING name, STRING value) { m_name=name; m_value=value; }

private:
	STRING m_name;
	STRING m_value;
};


}
#endif
