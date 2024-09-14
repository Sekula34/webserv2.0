#ifndef SERVERSETTINGS_HPP
#define SERVERSETTINGS_HPP
#include "DefaultSettings.hpp"
#include "Directive.hpp"
#include <ostream>
#include <vector>
#include "LocationSettings.hpp"

class LocationSettings;

//can change default Settings autoindex, error_page
//client MaxBodySize, index
//contains Location
class ServerSettings : public DefaultSettings
{
	private: 
		void 							_applyAllServerLevelDirectives();
		void 							_generateDefaultLocation(std::vector<LocationSettings>& serverLocation);
		bool							_hasDefaultLocation(const std::vector<LocationSettings>& serverLocation) const;
	protected: 
		int _serverId;
		std::vector<Directive>			_serverDirectives;
		std::vector<Token>				_serverTokens;
		std::vector<LocationSettings>	_serverLocations;
		
		
		std::vector<LocationSettings>	_setServerLocations();
	public :
		ServerSettings();
		ServerSettings(int serverId, DefaultSettings& settings, std::vector<Token>& allTokens);
		ServerSettings(const ServerSettings& source);
		ServerSettings& operator=(const ServerSettings& source);
		~ServerSettings();


		/**
		 * @brief Get the Location Part Of Url object
		 * if url is /hej/i/am/url/that/is/in/server function will return location that server have
		 * for examle if /hej/i/am/ location in config file it will return that
		 * @param url 
		 * @return std::string 
		 */
		std::string										getLocationURIfromPath(const std::string& fullPath) const;
		bool											amIServerLocation(const std::string& path) const;
		// std::vector<Token>								getServerTokens(void) const; // FIXME: FUNTIONS FOR TESTING OR NOT BEING USED
		// std::vector<Directive>							getServerDirectives(void) const; // FIXME: FUNTIONS FOR TESTING OR NOT BEING USED
		const std::vector<LocationSettings>&			getServerLocations() const;
		std::vector<LocationSettings>::const_iterator	fetchLocationWithUri(const std::string uri, bool& found) const;
		//LocationSettings& fetchLocationFromServer(std::string requestedLocation);
		const int&										getServerId() const;
		void											printServerSettings(void) const;
		void											printServerTokens(void) const;
		// void											addDirectiveToServer(Directive directive); // FIXME: FUNTIONS FOR TESTING OR NOT BEING USED

		//MR_NOTE: Be careful with this one! I think we cannot use friend.
		friend std::ostream& operator<<(std::ostream& os, const ServerSettings& server);
};

#endif
