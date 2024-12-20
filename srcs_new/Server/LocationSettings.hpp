#ifndef LOCATIONSETTINGS_HPP
# define LOCATIONSETTINGS_HPP
#include "DefaultSettings.hpp"
#include "../Parsing/Directive.hpp"
#include <ostream>
#include <vector>


class VirtualServer; 
class LocationSettings : public DefaultSettings
{

	//maybe should be directive
	public :
		const std::vector<Directive>&	getLocationDirectives(void) const;
		const std::string& 				getLocationUri(void) const;
		bool							isCgiLocation(void) const;
		bool							isCgiExtensionSet(const std::string& scriptExtension) const;

										LocationSettings();
										LocationSettings(const DefaultSettings& settings, const Token& locationToken, std::vector<Token>& serverTokens);
										LocationSettings(const DefaultSettings& settings, std::vector<Token>& serverTokens);
										LocationSettings(const LocationSettings& source);
		LocationSettings&				operator=(const LocationSettings& source);
										~LocationSettings();
	private :
		std::string 			_getUriFromToken(const Token& token);
		std::vector<Directive>	_setLocationDirectives();

		std::string 			_uri;
		Token 					_locationToken;
		std::vector<Directive>	_locationDirectives;
		std::vector<Token>		_parentServerTokens;
		// const VirtualServer&	_locationServer;

	public :
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
		friend std::ostream& operator<<(std::ostream& os, const LocationSettings& location);

};


#endif
