#include "peel/XmlParser.h"

using namespace std;
using namespace peel;

#include "peel/profile.h"

XmlParser* XmlParser::m_instance = NULL;


XmlParser::XmlParser(FILE *fp)
{
	m_position    = 0;
	m_buffer      = NULL;
	m_fp          = fp;
	m_useFile     = true;
	m_error       = false;
	m_errorString = _T("");
}

XmlParser::XmlParser(STRING *s)
{
	m_position    = 0;
	m_buffer      = s;
	m_fp          = NULL;
	m_useFile     = false;
	m_error       = false;
	m_errorString = _T("");
}

void XmlParser::createInstance(FILE *fp)
{
	if(XmlParser::m_instance!=NULL)
		delete XmlParser::m_instance;

	XmlParser::m_instance = new XmlParser(fp);
}

void XmlParser::createInstance(STRING *s)
{
	if(XmlParser::m_instance!=NULL)
		delete XmlParser::m_instance;

	XmlParser::m_instance = new XmlParser(s);
}


XmlEntity* XmlParser::getTag(long &offset)
{
	char c;

	if(m_useFile)
	{
		if(offset != m_position)
			fseek(m_fp, offset, SEEK_SET);

		if(m_fp == NULL)
		{
			error(_T("No file handle"));
			return NULL;
		}
		if(feof(m_fp)) 
		{
			error(_T("End of file"));
			return NULL;
		}
	}
	else
	if(m_buffer==NULL)
	{
		error(_T("No data"));
		return NULL;
	}


	// Skip whitespace
	//a=m_tag->find_first_not_of("\r\n\t ",offset);

	bool skipwhite = true;
	bool readtag = false;

	STRING tmp=_T("");
	STRING lit=_T("");

	while(1)
	{
		// Get a character from the string/file
		if(m_useFile)
		{
			if(feof(m_fp)) return NULL;
			c = fgetc(m_fp);
			offset++;
			m_position++;
		}
		else
		{
			if(offset >= (long)m_buffer->size()) break;
			c = m_buffer->at(offset++);
		}

		// Skip initial whitespace
		if(skipwhite && (c == '\r' || c == '\n' || c == ' ' || c == '\t'))
			continue;

		skipwhite = false;

		if(c=='<')
		{
			if(readtag)
			{
				error(_T("Unexpected '<'"));
				return NULL;
			}
			readtag = true;
			if(lit.size() >0)
			{
				offset--;
				return new XmlValueEntity(lit);
			}
			continue;
		}

		if(readtag)
		{
			if(c==_T('>'))
			{
				XmlTagEntity *x = new XmlTagEntity(offset);
				STARTCLOCK(2);
				if(!x->parseTag(tmp))
				{
					STOPCLOCK(2);
					delete (XmlEntity*) x;
					error(x->m_errorString);
					return NULL;
				}
				STOPCLOCK(2);
				return (XmlEntity*)x;
			}


			tmp.push_back(c);
		}
		else
			lit.push_back(c);

	}


	error(_T("No tags found"));
	return NULL;
}


void XmlParser::error(STRING s)
{
	m_error = true;
	m_errorString = s;
}

