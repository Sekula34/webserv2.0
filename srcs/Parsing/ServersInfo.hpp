#ifndef SERVERSINFO_HPP
# define SERVERSINFO_HPP
#include "Directive.hpp"
# include "ServerSettings.hpp"
#include <vector>
#include "../Client/Client.hpp"

/**
 * @brief class that contains all ServerSetting in vector _servers
 * 
 */
class ServersInfo 
{
	private :
		std::vector<Token> _allTokens;
		std::vector<ServerSettings> _servers;
		std::vector<Directive> _httpDirectives;
		int _numberOfServers;


		void _setHttpDirectives(void);
		bool _isTokenHttpDirective(const Token& toCheck) const;
		bool _validateClientHeader(const ClientHeader* header) const;
		std::vector<ServerSettings> _getAllServersIdWithPort(int port) const;

	public :
		ServersInfo(std::string configPath = "Configuration/config_files/simple_nginx_conf.conf");
		ServersInfo(const ServersInfo& source);
		ServersInfo& operator=(const ServersInfo& source);
		~ServersInfo();

		void printAllServersInfo(void) const;
		const std::vector<ServerSettings>& getAllServers(void) const;
		/**
		 * @brief Get the Server By Id object
		 * 
		 * @param serverId id = index + 1;
		 * @return const ServerSettings& 
		 */
		const ServerSettings& getServerById(int serverId) const;
		
		/**
		 * @brief Get the Server By PortNuber 
		 * 
		 * @param portNumber 
		 * @param serverName 
		 * @return const ServerSettings& first server that have port == PortNumber 
		 *if serverName is specified find return first Server with port == portNumber and name == serverName, if not again first with port is retured
		 */
		const ServerSettings* getServerByPort(int portNumber, std::string serverName ="") const;

		/**
		 * @brief get Server resposible for client response
		 * 
		 * @param client for which you are looking for server
		 * @return ServerSettings* for client or NULL if there is no Server for that client (400 bad request usually)
		 */
		const ServerSettings* getClientServer(const Client& client) const;
		
		/**
		 * @brief Get the Unique Ports number for creating listeningSockets
		 * 
		 * @return const std::vector<int> 
		 */
		const std::vector<int> getUniquePorts(void) const;

};

#endif
