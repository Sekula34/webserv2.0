#ifndef NGINXRETURN_HPP
# define NGINXRETURN_HPP
#include <ostream>
# include <string>

class NginnxReturn
{
	public :
		const bool&			getFlag(void) const;
		const int&			getStatus(void) const;
		const std::string&	getRedirectPath(void) const;
		void 				setFlag(bool state);
		void				setStatus(int status);
		void				setRedirectPath(std::string uri);
		// void				printNginxReturnInfo(void) const;

							NginnxReturn();
							NginnxReturn(bool flag, int status, std::string _redirectPath);
							NginnxReturn(const NginnxReturn& source);
		NginnxReturn& 		operator=(const NginnxReturn& source);
							~NginnxReturn();
	private :
		bool				_flag;
		int					_status;
		std::string			_redirectPath;
	public:;
		friend std::ostream& operator<<(std::ostream& os, const NginnxReturn& redirect);
};

#endif
