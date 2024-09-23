#ifndef DUMMYMESSAGE_HPP
# define DUMMYMESSAGE_HPP

# include <string>
# include <map>

class DummyMessage
{
	public:
		// Methods
		std::map<std::string, std::string>&		getHeader();
		std::string&	getBody();
		void			setHeader(std::map<std::string, std::string>& header);
		void			setBody(std::string& body);
		int&			getErrorCode();
		void			printMessage();
		// Attributes

	private:
		// Attributes
		std::map<std::string, std::string>	_header;
		std::string		_body;
		int				_errorCode;

	public:
		DummyMessage();
		~DummyMessage();
};

#endif
