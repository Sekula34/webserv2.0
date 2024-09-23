# include "FdData.hpp"

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

FdData::FdData(int fd, e_fdType type) : fd(fd), type(type), state(NONE)
{}
FdData::~FdData(void)
{}
FdData::FdData(FdData const & src) : fd(src.fd), type(src.type), state(src.state)
{}
FdData &		FdData::operator=(FdData const & rhs)
{
	*this = rhs;
	return (*this);
}