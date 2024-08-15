#ifndef URLSUFFIX_HPP
# define URLSUFFIX_HPP
#include <string>

/**
 * @brief class that store parse all UrlSuffix info
 * 
 */
class UrlSuffix 
{
	public:
		UrlSuffix(const std::string urlSuffix);
		UrlSuffix(const UrlSuffix& source);
		UrlSuffix& operator=(const UrlSuffix source);
		~UrlSuffix();

		const std::string& getPath() const;
		const std::string& getQueryParameters() const;
		const std::string& getFragment() const;
		const bool& isUrlSuffixValid() const;

	private:
		std::string _path; //can only be ?
		std::string _queryParameters;
		std::string _fragment;

		bool _valid;

		const std::string _urlSuffix;

		void _initVars();
		void _setAllVars();
		void _parseAndSetPath();
		void _parseAndSetQuerryParameters();
		void _parseAndSetFragment();
};

#endif
