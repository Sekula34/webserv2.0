#ifndef CLIENTHEADERMANAGER_HPP
# define CLIENTHEADERMANAGER_HPP
#include "ClientHeader.hpp"
#include <exception>
#include <vector>


class ClientHeaderManager
{
	private :
		std::vector<ClientHeader> _unreadHeaders;
		std::vector<ClientHeader> _readHeaders;

	public :
		ClientHeaderManager();
		ClientHeaderManager(const ClientHeaderManager& source);
		ClientHeaderManager& operator=(const ClientHeaderManager& source);
		~ClientHeaderManager();
		
		/**
		 * @brief Create a New Client Header object and add it to _unreadHeadersVector
		 * 
		 * @param clientFd 
		 */
		void createNewClientHeader(int& clientFd);

		/**
		 * @brief Get the Client Header object
		 * 
		 * @param clientFd fd of client you wnat to find
		 * @throw ClientHeaderNotFound if no such 
		 * @return ClientHeader& 
		 */
		ClientHeader& getClientHeader(int clientFd);

		/**
		 * @brief try to read ClientHeader with clientFd
		 * if there is no object with client FD create one and then try to read 
		 * @param clientFD
		 * @return ReadStatus 
		 */
		ReadStatus readClientHeader(int& clientFD);


		class ClientHeaderNotFound : public std::exception
		{
			public :
				const char * what() const throw();
		};


};

#endif
