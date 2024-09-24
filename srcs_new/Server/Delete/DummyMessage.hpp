#ifndef DUMMYMESSAGE_HPP
# define DUMMYMESSAGE_HPP

# include <string>
# include <map>
# include <iostream>

// For testing
# define MESSAGE_HEADER "DummyMessageHeader.txt"
# define MESSAGE_BODY "DummyMessageBody.txt"

class DummyMessage
{
	public:
		// Methods
		std::map<std::string, std::string>&		getHeader();
		std::string&	getBody();
		void			setHeader(std::map<std::string, std::string>& header);
		void			setBody(std::string& body);
		int&			getErrorCode();
		// void			printMessage();
		// Attributes

	private:
		// Attributes
		std::map<std::string, std::string>	_header;
		std::string		_body;
		int				_errorCode;

	public:
		DummyMessage();
		DummyMessage(std::string type, int errorCode);
		~DummyMessage();

	// Overloaded insertion operator (for testing).
	friend std::ostream&	operator<<(std::ostream& out, const DummyMessage& message);
};

#endif
