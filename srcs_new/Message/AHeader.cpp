#include "AHeader.hpp"
#include "../Utils/Logger.hpp"
#include "../Parsing/ParsingUtils.hpp"
#include <map>
#include <ostream>
#include <string>
#include <vector>



AHeader::AHeader(int& erorCode)
:m_errorCode(erorCode)
{

}

AHeader::AHeader(const std::string& headerSection, int& erorCode)
: m_headerSection(headerSection),
m_errorCode(erorCode)
{
	// std::cout << "#### header section: " << headerSection << std::endl;
	if(_fillHeaderFieldMap(_getHeaderFields(m_headerSection)) == false)
	{
		Logger::error("Error while filling header field map", true);
		return;
	}
	if(_checkHeaderFields() == false)
	{
		Logger::error("checking header fields failed", "");
		return;
	}
}

AHeader::AHeader(const AHeader& source)
:m_headerSection(source.m_headerSection),
m_errorCode(source.m_errorCode)
{
	if(_fillHeaderFieldMap(_getHeaderFields(m_headerSection)) == false)
	{
		Logger::error("Error while filling header field map", "");
		return;
	}
	if(_checkHeaderFields() == false)
	{
		Logger::error("checking header fields failed", "");
		return;
	}
	std::stringstream ss;
	ss << *this;
	Logger::info("Header fields map is successfully filled", "\n" + ss.str());
}

AHeader& AHeader::operator=(const AHeader& source)
{
	(void) source;
	Logger::warning("Why is AHeader = operator called??", "");
	return *this;
}

AHeader::~AHeader()
{

}

const std::map<std::string, std::string>& AHeader::getHeaderFieldMap() const
{
	return m_headerFields;
}
void AHeader::setOneHeaderField(std::string key, std::string value)
{
	m_headerFields[key] = value;
}

void AHeader::p_setHttpStatusCode(int httpCode)
{
	if(m_errorCode != 0)
	{
		Logger::warning("Overwriting error code, old: ", m_errorCode);
		Logger::warning("New error code", httpCode);
	}
	m_errorCode = httpCode;
}

std::string AHeader::p_getAllHeaderFieldsAsString() const
{
	std::string headerFields;
	std::map<std::string, std::string>::const_iterator it;
	for(it = m_headerFields.begin(); it != m_headerFields.end(); it++)
	{
		headerFields += _getOneHeaderFieldAsString(it->first, it->second);
	}
	return headerFields;
}

bool AHeader::p_isHeaderField(const std::string& field) const
{
	std::map<std::string, std::string>::const_iterator it = m_headerFields.find(field);
	if(it != m_headerFields.end())
		return true;
	return false;
}


const int& AHeader::getHttpStatusCode(void) const
{
	return m_errorCode;
}


bool AHeader::isBodyExpected() const
{
	std::string s = "chunked"; //FIXME: what is this s used for?
	if(m_headerFields.find("Content-Length") != m_headerFields.end()
	|| (m_headerFields.find("Transfer-Encoding") != m_headerFields.end()
		&& m_headerFields.at("Transfer-Encoding") == "chunked"))
	{
		std::cout << "body should be read!" << std::endl;
		return true;
	}
	return false;
}


bool AHeader::_fillHeaderFieldMap(std::vector<std::string> plainHeaders)
{
	for(size_t i = 0; i < plainHeaders.size(); i++)
	{
		// std::cout << "header field: " << plainHeaders[i] << std::endl;
		if(_setOneHeaderField(plainHeaders[i]) == false)
			return false;
	}
	return true;
	
}

std::vector<std::string> AHeader::_getHeaderFields(const std::string& header_str) const
{
	std::vector<std::string> headerFields =  ParsingUtils::splitString(header_str, "\r\n");
	headerFields.erase(headerFields.end() - 1);
	return headerFields;
}

std::string AHeader::_getOneHeaderFieldAsString(std::string key, std::string value) const 
{
	std::string oneHeaderField;
	oneHeaderField += key;
	oneHeaderField += ": ";
	oneHeaderField += value;
	oneHeaderField += "\r\n";
	return oneHeaderField;
}

bool AHeader::_setOneHeaderField(std::string keyAndValue)
{
	std::string key = ParsingUtils::extractUntilDelim(keyAndValue, ":");
	if(key != "")
		key.erase(key.end() - 1);
	std::string plainKey = ParsingUtils::getHttpPlainValue(key);
	std::string value = ParsingUtils::extractAfterDelim(keyAndValue, ":");
	value = ParsingUtils::getHttpPlainValue(value);
	if(key == "" || value == "")
	{
		p_setHttpStatusCode(500);
		Logger::warning("Something is off", "");
		return false;
	}
		//std::string key = connected[0];
		//std::string value = ParsingUtils::getHttpPlainValue(connected[1]);
	m_headerFields[key] = value;
	return true;
}

bool AHeader::_checkHeaderFields(void)
{
	//check if there is authorization
	if(p_isHeaderField("Authorization") == true)
	{
		Logger::error("Authorization is not supported", true);
		p_setHttpStatusCode(403);
		return false;
	}
	if(p_isHeaderField("Transfer-Encoding") == true && p_isHeaderField("Content-Length") == true)
	{
		Logger::error("Header contains both Content-Length and Transfer-Encoding which is contradiction", "");
		p_setHttpStatusCode(400);
		return false;
	}
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
