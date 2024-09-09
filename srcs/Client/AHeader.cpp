#include "AHeader.hpp"
#include "../Utils/Logger.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include <map>
#include <ostream>
#include <string>
#include <vector>

AHeader::AHeader(const std::string& headerSection)
: m_headerSection(headerSection)
{
	if(_fillHeaderFieldMap() == false)
	{
		Logger::error("Error while filling header field map", true);
		return;
	}
	Logger::info("Header fileds map is successfully filled", true);
}

AHeader::AHeader(const AHeader& source)
:AHeader(source.m_headerSection)
{

}

AHeader& AHeader::operator=(const AHeader& source)
{
	(void) source;
	Logger::warning("Why is AHeader = operator called??");
	return *this;
}

AHeader::~AHeader()
{

}

const std::map<std::string, std::string>& AHeader::getHeaderFieldMap() const
{
	return m_headerFields;
}


bool AHeader::_fillHeaderFieldMap()
{
	std::vector<std::string> plainHeaders = _getHeaderFields();
	for(size_t i = 0; i < plainHeaders.size(); i++)
	{
		if(_setOneHeaderField(plainHeaders[i]) == false)
			return false;
	}
	return true;
	
}

std::vector<std::string> AHeader::_getHeaderFields() const
{
	std::vector<std::string> headerFields =  ParsingUtils::splitString(m_headerSection, "\r\n");
	headerFields.erase(headerFields.end() - 1);
	return headerFields;
}

bool AHeader::_setOneHeaderField(std::string keyAndValue)
{
	std::vector<std::string> connected = ParsingUtils::splitString(keyAndValue, ':');
	if(connected.size() < 2)
	{
		Logger::warning("Header filed syntax is in client Header is invalid ");
		return false;
	}
	std::string key = connected[0];
	std::string value = ParsingUtils::getHttpPlainValue(connected[1]);
	m_headerFields[key] = value;
	return true;
}


std::ostream& operator<<(std::ostream& os, const AHeader& header)
{
	std::map<std::string, std::string>::const_iterator it = header.m_headerFields.begin();
	for(;it != header.m_headerFields.end(); it++)
	{
		os << it->first << ": " << it->second << std::endl;
	}
	return os;
}
