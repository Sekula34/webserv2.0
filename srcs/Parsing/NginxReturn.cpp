#include "NginxReturn.hpp"
#include <iostream>
#include <stdexcept>

NginnxReturn::NginnxReturn()
:_flag(false), _status(0), _redirectPath("DEFAULT REDIRECT Path")
{

}
NginnxReturn::NginnxReturn(bool flag, int status, std::string _redirectPath)
:_flag(flag), _status(status), _redirectPath(_redirectPath)
{

}

NginnxReturn& NginnxReturn::operator=(const NginnxReturn& source)
{
	_flag = source._flag;
	_status = source._status;
	_redirectPath = source._redirectPath;
	return (*this);
}

NginnxReturn::NginnxReturn(const NginnxReturn& source)
{
	(*this) = source;
}

NginnxReturn::~NginnxReturn()
{

}

bool NginnxReturn::getFlag() const
{
	return _flag;
}

int NginnxReturn::getStatus() const 
{
	return (_status);
}

std::string NginnxReturn::getRedirectPath() const 
{
	return (_redirectPath);
}

void NginnxReturn::printNginxReturnInfo(void) const 
{
	std::cout << "Nginx Return is: " << _flag << std::endl;
	if(_flag)
	{
		std::cout << "Redirect code is " << _status << std::endl;
		std::cout << "Redirect page is " << _redirectPath << std::endl;
	} 
}

void NginnxReturn::setFlag(bool state)
{
	_flag = state;
}

void NginnxReturn::setStatus(int status)
{
	if(status < 100 || status > 599)
	{
		throw std::runtime_error("Status of nginxReturn is out of range");
	}
	_status = status;
}

void NginnxReturn::setRedirectPath(std::string path)
{
	if(path.empty())
		throw std::runtime_error("Trying to set empty redirect Path");
	_redirectPath = path;
}