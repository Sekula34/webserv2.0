#ifndef LOCATIONSETTINGS_HPP
# define LOCATIONSETTINGS_HPP
#include "DefaultSettings.hpp"
#include "Directive.hpp"
//#include "ServerSettings.hpp"
#include <vector>


class LocationSettings : public DefaultSettings
{

	//maybe should be directive
	private :
		std::string _uri;
		Token _locationToken;
		std::vector<Directive> _locationDirectives;
		std::vector<Token> _parentServerTokens;

		std::string _getUriFromToken(const Token& token);
		std::vector<Directive> _setLocationDirectives();
	public :
		LocationSettings();
		LocationSettings(const DefaultSettings& settings, const Token& locationToken, std::vector<Token>& serverTokens);
		LocationSettings(const LocationSettings& source);
		LocationSettings& operator=(const LocationSettings& source);
		~LocationSettings();


		std::vector<Directive> getLocationDirectives(void) const;
		const std::string& getLocationUri(void) const;
		void printLocationSettings(void) const;
		static void printAllLocationSettings(std::vector<LocationSettings>& allLocations);

		/**
		 * @brief Set the Index Page Path object
		 * 
		 * @param pathToIndex where the path will be setted
		 * @return true if file exist and put path to index in param
		 * @return false if file dont exist, path to index will be ""
		 */
		bool setIndexPagePath(std::string& pathToIndex) const;
		bool isCgiLocation(void) const;
};

class FindByUri
{
	private : 
		std::string uri;
	public :
		FindByUri(const std::string& u) : uri(u) {}

		bool operator()(const LocationSettings& location) const
		{
			return (location.getLocationUri() == uri);
		}
};

#endif
