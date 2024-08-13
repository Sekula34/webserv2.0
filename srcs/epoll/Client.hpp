
#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <string>
# include <ctime>
// #define MAXLINE			4096
// #define MAXLINE			493
# define MAXLINE			50
# define MAX_TIMEOUT		10000
# include "../Client/ClientHeader.hpp"
# include "CgiProcessor.hpp"

class CgiProcessor;
# include "../Response/Response.hpp"


class Client {

	public:
		static int			client_cntr;
		
							// canonical
							Client (int const fd, int const epollfd);
							~Client(void);
							Client(Client const & src);
		Client &			operator=(Client const & rhs);
							
							// set and get
		Response* 			getResponse() const;
		void				setResponse(Response* respose);
		unsigned long		getId() const;
		int					getFd() const;
		std::clock_t		getStartTime() const;
		std::string			getMessage() const;
		unsigned char*		getRecvLine() const;
		int					getEpollFd() const;
		int					getErrorCode() const;
		bool				getReadHeader() const;
		bool				getReadBody() const;
		bool				getWriteClient() const;
		std::string const &	getBody() const;
		void				setErrorCode(int e);
		void				setReadHeader(bool b);
		void				setReadBody(bool b);
		void				setWriteClient(bool b);

							//Client specific functions
		void				addRecvLineToMessage();
		bool				check_timeout() const;

		/**
		 * @brief Create a Client Header that is stored in _header and should be deleted in destructor
		 * 
		 */
		void				createClientHeader();
		CgiProcessor*		Cgi;			
		ClientHeader*		header; //client Responsible for deleting

	private:
		int					_errorCode;
		unsigned long const	_id;
		int const			_fd;
		std::clock_t const	_start;
		int const			_epollfd;
		std::string			_message;
		std::string			_client_body; //maybe this will be replaced by body class
		std::string			_response_body; // this will be replaced by respnse class
		unsigned char*			_recvline;
		bool				_readheader;
		bool				_readbody;
		bool				_writeclient;
							Client(void);

		Response* _response; // client owns so it should delete

		void	_initVars(void);
};

#endif
