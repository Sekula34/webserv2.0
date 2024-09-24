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
	public :
		static void						checkDuplicateDirectives(const std::vector<Directive>& dirVec);
		bool							isMethodAllowed(std::string method) const;
		void 							setListenPort(int listenPort);
		void 							setErrorPage(int errorCode, std::string path);
		void 							setAllAcceptedMethodsToFalse(void);
		void 							setAcceptedMethodToTrue(std::string methodName);
		void 							setClientMaxBodySize(size_t maxSize);
		void 							setAutoIndex(bool state);
		void 							setNginxReturn(int statusCode, std::string redirectPath, bool flag = 1);
		void 							setIndexes(std::vector<std::string> indexes);
		void 							setRoot(std::string root);
		void 							setServerName(std::string serverName);
		void 							setCgiExtensions(std::vector<std::string> extensionsVector);
		const int&						getPort(void) const;
		const std::string& 				getRoot(void) const;
		const std::string& 				getServerName(void) const;
		const NginnxReturn&				getNginxReturn(void) const;
		const bool& 					getAutoindexFlag(void) const;
		const std::vector<std::string>&	getIndexes() const;
		/**
		 * @brief Get the Error Page Path
		 * 
		 * @param errorCode which error are you searching for
		 * @return const std::string path of errorPage path or "" if there is no such path
		 */
		const std::string 				getErrorPagePath(const int errorCode) const;
		const std::vector<std::string>&	getCgiExtensions(void) const;

										DefaultSettings(void);
										DefaultSettings(const DefaultSettings& source);
		DefaultSettings& 				operator=(const DefaultSettings& source);
										~DefaultSettings();

	protected :
		//server stuff 
		std::string					p_serverName; // This is host
		int 						p_listenPort;
		std::string					p_host; //TODO remove this it is replace with host
		//location stuff
		std::map<int, std::string>	p_errorPages;
		std::map<std::string, bool>	p_acceptedMethods;// limitExcept
		std::size_t					p_clientMaxBody;
		bool 						p_autoindex;
		NginnxReturn 				p_return;
		std::vector<std::string>	p_index;
		std::string					p_root;
		std::vector<std::string>	p_cgiExtensions;
	private :
		void						_setDefaultHttpMethods(void);
		void						_setDefaultIndexes(void);


	public:
		friend std::ostream& operator<<(std::ostream& os, const DefaultSettings& server);
};


#endif
