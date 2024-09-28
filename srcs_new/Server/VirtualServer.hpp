#ifndef VIRTUALSERVER_HPP
#define VIRTUALSERVER_HPP
#include "DefaultSettings.hpp"
#include "../Parsing/Directive.hpp"
#include <ostream>
#include <vector>
#include "LocationSettings.hpp"

class LocationSettings;

//can change default Settings autoindex, error_page
//client MaxBodySize, index
//contains Location
class VirtualServer : public DefaultSettings
{
	public:
		/**
		 * @brief Get the Location Part Of Url object
		 * if url is /hej/i/am/url/that/is/in/server function will return location that server have
		 * for examle if /hej/i/am/ location in config file it will return that
		 * @param url 
		 * @return std::string 
		 */
		std::string										getLocationURIfromPath(const std::string& fullPath) const;
		bool											amIServerLocation(const std::string& path) const;
		const std::vector<Token>&						getServerTokens(void) const;
		const std::vector<Directive>&					getServerDirectives(void) const;
		const std::vector<LocationSettings>&			getServerLocations() const;
		std::vector<LocationSettings>::const_iterator	fetchLocationWithUri(const std::string uri, bool& found) const;
		const int&										getServerId() const;
		void											addDirectiveToServer(Directive directive);

														VirtualServer();
														VirtualServer(int serverId, DefaultSettings& settings, std::vector<Token>& allTokens);
														VirtualServer(const VirtualServer& source);
		VirtualServer& 								operator=(const VirtualServer& source);
														~VirtualServer();
	private: 
		void 							_applyAllServerLevelDirectives();
		void 							_generateDefaultLocation(std::vector<LocationSettings>& serverLocation);
		bool							_hasDefaultLocation(const std::vector<LocationSettings>& serverLocation) const;
		const std::vector<Directive>    _getServerLevelDirectives() const;
		std::vector<LocationSettings>	_setServerLocations();

		int 							_serverId;
		std::vector<Directive>			_serverDirectives;
		std::vector<Token>				_serverTokens;
		std::vector<LocationSettings>	_serverLocations;


	public:
		friend std::ostream& operator<<(std::ostream& os, const VirtualServer& server);
};

#endif
