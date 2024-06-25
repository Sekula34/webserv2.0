#ifndef CLIENTHEADER_HPP
# define CLIENTHEADER_HPP
#include <string>

/**
 * @brief enum used for return read Status 
 * 
 */
enum ReadStatus
{
	DONE,
	CONTINUE_READING,
	ERROR = -1
};


const int BUFFER_SIZE = 4096;

/**
 * @brief class for reading and storing client Message, one read at the time
 * 
 */
class ClientHeader 
{

	private :
		int& _clientFd;
		std::string _message;
		bool _fullyRead;
		ClientHeader();

	public :
		ClientHeader(int& clientFd);
		ClientHeader(const ClientHeader& source);
		ClientHeader& operator=(const ClientHeader& source);
		~ClientHeader();

		/**
		 * @brief 
		 * 
		 * @return ReadStatus DONE if END OF HEADER IS RECEIVED, CONTINUE READING if buffer size is full but end of header is not recevied
		 * ERROR if read Failes or received message is without header
		 */
		ReadStatus readOnce();

		const int& getClientFd() const;
		bool isFullyRead() const;

		friend std::ostream& operator<<(std::ostream& os, const ClientHeader& obj);
};


class FindClientByFd {

private:
    int search_fd;
public:
    FindClientByFd(int fd) : search_fd(fd) {}

	//this make class a functor(class that defines () method )
    bool operator()(const ClientHeader& clientheader) const
	{
        return clientheader.getClientFd() == search_fd;
    }
};
#endif
