#ifndef NGINXRETUNR_HPP
# define NGINXRETUNR_HPP
# include <string>

class NginnxReturn
{
	private :
		bool _flag;
		int _status;
		std::string _redirectPath;
	public :
		NginnxReturn();
		NginnxReturn(bool flag, int status, std::string _redirectPath);
		NginnxReturn(const NginnxReturn& source);
		NginnxReturn& operator=(const NginnxReturn& source);
		~NginnxReturn();


		bool getFlag(void) const;
		int getStatus(void) const;
		std::string getRedirectPath(void) const;

		void setFlag(bool state);
		void setStatus(int status);
		void setRedirectPath(std::string uri);
		void printNginxReturnInfo(void) const;
};

#endif
