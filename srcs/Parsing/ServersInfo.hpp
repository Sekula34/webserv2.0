#ifndef SERVERSINFO_HPP
# define SERVERSINFO_HPP
#include "Directive.hpp"
# include "ServerSettings.hpp"
#include <vector>

class ServersInfo 
{
	private :
		std::vector<Token> _allTokens;
		std::vector<ServerSettings> _servers;
		std::vector<Directive> _httpDirectives;
		int _numberOfServers;


		void _setHttpDirectives(void);
		bool _isTokenHttpDirective(const Token& toCheck) const;

	public :
		ServersInfo(std::string configPath = "Configuration/config_files/simple_nginx_conf.conf");
		ServersInfo(const ServersInfo& source);
		ServersInfo& operator=(const ServersInfo& source);
		~ServersInfo();

		void printAllServersInfo(void) const;
		const std::vector<ServerSettings>& getAllServers(void) const;
		const ServerSettings& getOneServer(int serverIndex) const;

};

#endif
