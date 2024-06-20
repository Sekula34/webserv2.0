#ifndef SERVERSETTINGS_HPP
#define SERVERSETTINGS_HPP
#include "DefaultSettings.hpp"
#include "Directive.hpp"
#include <vector>
#include "LocationSettings.hpp"

class LocationSettings;

//can change default Settings autoindex, error_page
//client MaxBodySize, index
//contains Location
class ServerSettings : public DefaultSettings
{
	private: 
		void _applyAllServerLevelDirectives();

	protected: 
		int _serverId;
		std::vector<Directive> _serverDirectives;
		std::vector<Token> _serverTokens;
		std::vector<LocationSettings> _serverLocations;
		
		
		std::vector<LocationSettings> _setServerLocations();
	public :
		ServerSettings();
		ServerSettings(int serverId, DefaultSettings& settings, std::vector<Token>& allTokens);
		ServerSettings(const ServerSettings& source);
		ServerSettings& operator=(const ServerSettings& source);
		~ServerSettings();

		std::vector<Token> getServerTokens(void) const;
		std::vector<Directive> getServerDirectives(void) const;
		std::vector<LocationSettings> getServerLocations() const;
		void printServerSettings(void) const;
		void printServerTokens(void) const;
		void addDirectiveToServer(Directive directive);
};

#endif
