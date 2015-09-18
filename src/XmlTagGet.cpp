#include "peel/XmlTag.h"


using namespace peel;

STRING XmlTagEntity::getArgs()
{
	OSTREAM ss;
	for(size_t i=0;i<m_arguments.size(); i++)
		ss << m_arguments.at(i).first << _T("=") << m_arguments.at(i).second << _T(" ");

	return ss.str();
}

bool XmlTagEntity::getArg(STRING name, STRING &value)
{
	for(size_t i=0;i<m_arguments.size(); i++)
	{
		if(PU::StrICmp(m_arguments.at(i).first,name))
		{
			value = m_arguments.at(i).second;
			return true;
		}
	}

	value=_T("");
	return false;
}

bool XmlTagEntity::getArg(STRING name, long &lvalue)
{
	STRING tmp;
	if(!getArg(name, tmp))
		return false;
#ifdef _WIN32
	lvalue = _tstol(tmp.c_str());
#else
	lvalue = atol(tmp.c_str());
#endif
	return true;
}


bool XmlTagEntity::getArg(STRING name, int &ivalue)
{
	STRING tmp;
	if(!getArg(name, tmp))
		return false;
#ifdef _WIN32
	ivalue = _tstoi(tmp.c_str());
#else
	ivalue = atoi(tmp.c_str());
#endif
	return true;
}

bool XmlTagEntity::getArg(STRING name, unsigned int &ivalue)
{
	int i=0;
	if(!getArg(name, i))
		return false;

	if(i<=0) 
		return false;

	ivalue=i;
	
	return true;
}

bool XmlTagEntity::getArg(STRING name, float &fvalue)
{
	STRING sval;

	if(!getArg(name, sval))
		return false;

	ISTREAM ss(sval);

	return (ss >> fvalue);
}

bool XmlTagEntity::getArg(STRING name, double &dvalue)
{
	STRING sval;

	if(!getArg(name, sval))
		return false;

	ISTREAM ss(sval);

	return (ss >> dvalue);
}

bool XmlTagEntity::getArg(STRING name, bool &bvalue)
{
	STRING sval;
	if(!getArg(name, sval))
		return false;

	if(sval==_T("1") || PU::StrICmp(sval,_T("true")))
	{
		bvalue = true;
		return true;
	}
	else
	if(sval==_T("0") || PU::StrICmp(sval,_T("false")))
	{
		bvalue = false;
		return true;
	}
	return false;
}

