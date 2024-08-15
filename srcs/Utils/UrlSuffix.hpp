#ifndef URLSUFFIX_HPP
# define URLSUFFIX_HPP
#include <string>

class UrlSuffix 
{
	public:
		UrlSuffix(const std::string urlSuffix);
		UrlSuffix(const UrlSuffix& source);
		UrlSuffix& operator=(const UrlSuffix source);
		~UrlSuffix();

		const std::string& getPath() const;
		const std::string& getQueryParameters() const;


	private:
		std::string _path; //can only be ?
		std::string _queryParameters;
		std::string _fragment;

		const std::string _urlSuffix;

		void _initVars();
		void _parseAndSetPath();
		void _parseAndSetQuerryParameters();
		void _parseAndSetFragmetn();
};

#endif
