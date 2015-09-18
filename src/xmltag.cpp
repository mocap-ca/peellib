#include "peel/XmlTag.h"

#include "peel/XmlParser.h"

#include "peel/profile.h"

using namespace peel;


// Tag Entity, e.g. <ANIMAL TYPE="DOG" BREED="POODLE"><PET>Bubbles</PET></ANIMAL>
// tagName = "ANIMAL"
// m_subTags = "<PET>Bubbles</PET>"
// m_arguments: ("TYPE","DOG"), ("BREED","POODLE");

// The first item needs to be a tag and not a string literal, and this function will parse up to the
// closing tag.

// The input is a character string and an offset in that string.

// The function will recurse downwards and keep parsing *tag, creating XmlTagEntity and XmlValueEntity
// objects as it goes.  This will only work if all of the xml file is loaded in to memory, but that's
// not a good idea for very long files.



// The offset value is at the _end_ of this tag, which can be used 
// to find subtags

XmlTagEntity::XmlTagEntity(long offset)
{
	m_tag         = _T("");
	m_closing     = false;
	m_selfClosing = false;
	m_complete    = false;
	m_offset      = offset;
	m_pos         = offset;
	m_ready       = true;
	m_next        = 0;
	m_inserted    = Xml::npos;
	m_error       = false;
	m_errorString = _T("");
}


XmlTagEntity::~XmlTagEntity()
{
	for(size_t i=0; i< m_subTags.size(); i++)
		delete m_subTags[i];

	m_subTags.clear();
	m_arguments.clear();

}

// Parses a single child node.  Note that the offset must be
// sitting at a child note for this to work correctly.  All
// subTags should be parsed.

XmlEntity* XmlTagEntity::getNext()
{
	if(m_complete) return NULL;

	while(m_ready && !m_complete && !m_error)
		parseOne();

	if(!m_complete)
		parseSub();

	if(m_inserted < m_subTags.size())
		return m_subTags.at(m_inserted);
	else
		return NULL;
}

XmlTagEntity* XmlTagEntity::getNextTag()
{
	while(!m_complete && !m_error)
	{
		XmlEntity *x = getNext();
		if(x==NULL) return false;
		if(x->isTag()) return (XmlTagEntity*)x;
	}

	return NULL;
}



/*
 Parse a single subtag.  This tag must be in the state "ready", indicating
 that the current pointer is in a valid position (not in a subtag somewhere)

 The next tag may be:
     A closing tag for this tag, in which case set m_complete to true
     A sub tag, in which case store it and set m_ready to false, indicating
          that processing of subtags is required./

 After the tag is parsed the following conditions may occur:

 If
 */

long XmlTagEntity::parseOne()
{
	if(!m_ready) 
	{
		error(_T("Tag not ready while parsing"));
		return m_pos;
	}

	XmlParser* parser = XmlParser::m_instance;
	if(parser==NULL)
	{
		error(_T("could not create parser instance"));
		return m_pos;
	}

	STARTCLOCK(1);
	XmlEntity *tag = parser->getTag(m_pos);
	STOPCLOCK(1);

	m_ready = false;
	if(tag==NULL) 
	{
		if(parser->m_error)
			error(parser->m_errorString);
		else
			error(_T("Parse error during tag splitting"));
		return m_pos;
	}

	if(!tag->isTag())
	{
		m_ready = true;
		m_subTags.push_back(tag);
		m_inserted = m_subTags.size()-1;
	}
	else
	{
		XmlTagEntity* x = (XmlTagEntity*)tag;
		

		//cout << "      t: " << x->m_name << endl;
		if(!x->m_closing)
		{
			m_subTags.push_back(tag);
			m_inserted = m_subTags.size()-1;
		}

		if(x->m_selfClosing)
			m_ready = true;

		if(x->m_closing && x->isName(m_name))
		{
			// This tag is complete, no further parsing required.
			m_complete = true;
			m_ready = false;
		}
	}

	return m_pos;
}


void XmlTagEntity::error(STRING e)
{
	m_error = true;
	m_errorString = e;
}

// Parse the sub tags.  The sub tag list may not be complete as we may not have 
// filled it yet.  This will completely parse all subtags in the list.

long XmlTagEntity::parseSub()
{
	XmlTagEntity* x;

	//cout << "Parsing subtags for " << m_name << endl;

	if(m_ready)
	{
		error(_T("Sub tags not parsed"));
		return m_pos;
	}


	bool subComplete = true;

	if(m_inserted == Xml::npos)
	{
		error(_T("Invalid index"));
		return m_pos;
	}

	if(m_subTags.at(m_inserted)->isTag())
	{
		x = (XmlTagEntity*)m_subTags.at(m_inserted);
		while(!x->m_complete && !x->m_error)
			m_pos = x->parse();
	}

	if(subComplete)
		m_ready = true;


	return m_pos;
}

long XmlTagEntity::reParse()
{
	m_pos = m_offset;
	unParse();
	return parse();

}



// if m_ready is true, then the pointer is in a valid position for parsing
// tags.  The next tag may be a closing tag for me, or a sub tag - parseOne()
// will take care of this.  If after parseOne() the pointer is in a valid
// position for parsing my subtags, m_ready will return true, otherwise
// the pointer will be in a subtag somewhere and m_ready will return false

long XmlTagEntity::parse()
{
	while(!m_complete && !m_error)
	{
		while(m_ready && !m_error)
			parseOne();

		parseSub();
	} 
	return m_pos;
}

// Unparse free up the data asccociated with this tree
bool XmlTagEntity::unParse()
{
	for(size_t i=0; i< m_subTags.size(); i++)
		delete m_subTags[i];

	m_subTags.clear();
	m_complete = false;
	m_pos      = m_offset;
	m_ready    = true;

	return true;
}

XmlTagEntity* XmlTagEntity::findSubTag(STRING name)
{
	for(vector<XmlEntity*>::size_type i=0; i < m_subTags.size(); i++)
	{
		if(m_subTags.at(i)->isTag())
		{
			XmlTagEntity *ent = (XmlTagEntity *) m_subTags.at(i);
			if(PU::StrICmp(ent->m_name,name))
				return ent;
		}
	}
	return NULL;
}

STRING XmlTagEntity::getName()
{
	return m_name;
}

bool XmlTagEntity::isName(STRING s)
{
	if(PU::StrICmp(m_name,s)) return true;
	return false;
}

STRING XmlTagEntity::getSubValue()
{
	STRING ret;
	for(size_t i=0; i < m_subTags.size(); i++)
	{
		if(!m_subTags.at(i)->isTag())
			ret += ((XmlValueEntity*)m_subTags.at(i))->m_value;
	}
	return ret;
}

XmlEntity* XmlTagEntity::getSub(size_t pos)
{
	if(m_subTags.size() > pos) return NULL;
	return m_subTags.at(pos);
}

XmlTagEntity* XmlTagEntity::getSubTag(size_t pos)
{
	if(pos >= m_subTags.size()) return NULL;
	if(!m_subTags.at(pos)->isTag()) return NULL;
	return (XmlTagEntity*)m_subTags.at(pos);
}

size_t XmlTagEntity::subTags()
{
	while(!m_complete) m_offset = parseOne();
	return m_subTags.size();
}




// tag should be of the form:  ANIMAL TYPE="DOG" BREED="POODLE"  (brackets are removed)

bool XmlTagEntity::parseTag(STRING src)
{
	PL_CHAR  node[512];
	PL_CHAR  name[512];
	PL_CHAR  value[512];
	size_t node_i  = 0;
	size_t name_i  = 0;
	size_t value_i = 0;

	strpair p;
	bool quoted;
	bool first;
	bool skipquote = false;

	// st_node: ANIMAL
	// st_wsname: whitespace before a name tag
	// st_name: TYPE, BREED
	// st_value: DOG, POODLE
	enum mode { st_node, st_wsname, st_name, st_value, err } stat=st_node;

	size_t i=0;

	if(src.size()==0) return false;

	if(src.at(src.size()-1)=='/')
	{
		m_selfClosing = true;
		m_complete = true;
		src.erase(src.size()-1);
	}

	if(src.at(0) == '/')
	{
		m_closing=true;
		i++;
	}

	// skip whitespace
	for(; i < src.length() && isspace(src.at(i)); i++);

	char c = src.at(i);
	while(i < src.length())
	{
		c = src.at(i++);

		if(node_i ==511 || name_i==511 || value_i == 511)
		{
			error(_T("string out of bounds - 512 string limit"));
			return false;
		}

		switch(stat)
		{
		case st_node:

			// we are still in the tag name.  The string should be pre parsed for
			// <> so we don't need to check for them.
			STARTCLOCK(3);
			if(isspace(c))
			{
				node[node_i++] = '\0';
				m_name = node;
				stat=st_wsname;
			}				
			else
				node[node_i++] = c;
			STOPCLOCK(3);
			break;

		case st_wsname:
			if(isspace(c))
				continue;
			name[name_i++]=c;
			stat = st_name;
			break;

		case st_name:
			STARTCLOCK(4);
			if(c=='=')
			{
				first=true;
				quoted=false;
				skipquote=false;
				stat=st_value;
			}
			else
			if(isspace(c))
			{
				name[name_i++] = '\0';
				value[value_i++] = '\0';
				p.first=name;
				p.second=value;
				m_arguments.push_back(p);
				name_i=0;
				value_i=0;
				stat=st_wsname;
			}
			else
				name[name_i++] = c;

			STOPCLOCK(4);
 
			break;

		case st_value:
			STARTCLOCK(5);
			// Stop on first whitespace if unquoted, or ending quote.
			if((!quoted && isspace(c)) || (quoted && !skipquote && c=='\"'))
			{
				name[name_i++] = '\0';
				value[value_i++] = '\0';
				p.first=name;
				p.second=value;
				m_arguments.push_back(p);
				name_i=0;
				value_i=0;
				stat=st_wsname;
			}
			else
			{
				if(first)
				{
					first=false;
					if(c=='\"')
					{
						quoted=true;
						STOPCLOCK(5);
						continue;
					}
				}

				if(c=='\\')
				{
					// check the next value;
					char q = src.at(i);
//					if(q == '\\')
//					{
//						value[value_i++] = '\\';
//						i++;
//						continue;
//					}
//					else
					if(q == '"')
					{
						skipquote = true;
						i++;
						continue;
					}
				}

				value[value_i++]=c;
			}
			STOPCLOCK(5);
		}
	}

	if(i=src.length())
	{
		if(stat==st_node)
		{
			node[node_i]='\0';
			m_name=node;
		}

		if(stat==st_value)
		{
			name[name_i]='\0';
			value[value_i]='\0';
			p.first=name;
			p.second=value;
			m_arguments.push_back(p);
		}
	}
	
	return true;
}
