#ifndef DEFAULTSETTINGS_HPP
# define DEFAULTSETTINGS_HPP
#include <cstddef>
#include <map>
#include <string>
#include "NginxReturn.hpp"
#include <vector>
#include "Directive.hpp"

class DefaultSettings 
{
	private :
		void _setDefaultHttpMethods(void);
		void _setDefaultIndexes(void);

	protected :
		//server stuff 
		std::string _serverName;
		int _listenPort;
		std::string _host;
		
		
		//location stuff
		std::map<int, std::string> _errorPages;
		std::map<std::string, bool> _acceptedMethods;// limitExcept
		std::size_t _clientMaxBody;
		bool _autoindex;
		NginnxReturn _return;
		std::vector<std::string> _index;
		std::string _root;
		std::vector<std::string> m_cgiExtensions;


	public :
		DefaultSettings(void);
		DefaultSettings(const DefaultSettings& source);
		DefaultSettings& operator=(const DefaultSettings& source);
		~DefaultSettings();

		static void checkDuplicateDirectives(const std::vector<Directive>& dirVec);

		bool isMethodAllowed(std::string method) const;

		void setListenPort(int listenPort);
		void setErrorPage(int errorCode, std::string path);
		void setAllAcceptedMethodsToFalse(void);
		void setAcceptedMethodToTrue(std::string methodName);
		void setClientMaxBodySize(size_t maxSize);
		void setAutoIndex(bool state);
		void setNginxReturn(int statusCode, std::string redirectPath, bool flag = 1);
		void setIndexes(std::vector<std::string> indexes);
		void setRoot(std::string root);
		void setServerName(std::string serverName);
		void setCgiExtensions(std::vector<std::string> extensionsVector);
		//applydirective maybe

		void printAllSettings(void) const;

		const int& getPort(void) const;
		const std::string& getRoot(void) const;
		const std::string& getServerName(void) const;
		const NginnxReturn& getNginxReturn(void) const;
		const bool& getAutoindexFlag(void) const;
		const std::vector<std::string>& getIndexes() const;

		/**
		 * @brief Get the Error Page Path
		 * 
		 * @param errorCode which error are you searching for
		 * @return const std::string path of errorPage path or "" if there is no such path
		 */
		const std::string getErrorPagePath(const int errorCode) const;

		const std::vector<std::string>& getCgiExtensions(void) const;

		friend std::ostream& operator<<(std::ostream& os, const DefaultSettings& server);
};


#endif