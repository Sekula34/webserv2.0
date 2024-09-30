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
		/**
		 * @brief Set the Cgi Script Name object and set cgi scritp extension aswell
		 * 
		 * @param scriptWithExtension 
		 * @return true script name and extensions are valid
		 * @return false script extension is not valid
		 */
		bool setCgiScriptName(std::string scriptWithExtension);
		void setCgiPathInfo(std::string pathInfo);
		const std::string& getCgiScriptName() const;
		const std::string& getCgiScriptExtension() const;
		const std::string& getCgiPathInfo() const;

	private:
		std::string _path; //can only be ?
		std::string _queryParameters;
		std::string _cgiScriptName;
		std::string _cgiScriptExtenstion;
		std::string _cgiPathInfo;

		const std::string _urlSuffix;

		void _initVars();
		void _setAllVars();
		void _setCgiScriptExtension(std::string extension);
		void _parseAndSetPath();
		void _parseAndSetQuerryParameters();
};

#endif
