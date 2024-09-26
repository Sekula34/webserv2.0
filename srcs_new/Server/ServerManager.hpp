#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP
#include "../Parsing/Directive.hpp"
# include "VirtualServer.hpp"
#include <vector>
// #include "../Client/Client.hpp"

/**
 * @brief class that contains all ServerSetting in vector _servers
 * 
 */
class ServerManager 
{
	public:
		const std::vector<VirtualServer>&	getAllServers(void) const;
		/**
		 * @brief Get the Server By Id object
		 * 
		 * @param serverId id = index + 1;
		 * @return const VirtualServer& 
		 */
		const VirtualServer&				getServerById(int serverId) const;
		/**
		 * @brief Get the Server By PortNuber 
		 * 
		 * @param portNumber 
		 * @param serverName 
		 * @return const VirtualServer& first server that have port == PortNumber 
		 *if serverName is specified find return first Server with port == portNumber and name == serverName, if not again first with port is retured
		 */
		const VirtualServer*				getServerByPort(int portNumber, std::string serverName ="") const;

		/**
		 * @brief get Server resposible for client response
		 * 
		 * @param client for which you are looking for server
		 * @return VirtualServer* for client or NULL if there is no Server for that client (400 bad request usually)
		 */
		// TODO: This is an importnt function
		//const VirtualServer* getClientServer(const Client& client) const;

		/**
		 * @brief Get the Unique Ports number for creating listeningSockets
		 * 
		 * @return const std::vector<int> 
		 */
		const std::vector<int>				getUniquePorts(void) const;

											ServerManager(std::string configPath = "configuration_files/default.conf");
											ServerManager(const ServerManager& source);
		ServerManager& 						operator=(const ServerManager& source);
											~ServerManager();
	private:
		std::vector<Token>			_allTokens;
		std::vector<VirtualServer>	_servers;
		std::vector<Directive>		_httpDirectives;
		int							_numberOfServers;

		void 						_setHttpDirectives(void);
		bool 						_isTokenHttpDirective(const Token& toCheck) const;
		// bool _validateRequestHeader(const RequestHeader* header) const;
		std::vector<VirtualServer>	_getAllServersIdWithPort(int port) const;
};

#endif
