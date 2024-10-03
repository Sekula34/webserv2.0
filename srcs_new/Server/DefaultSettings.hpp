#ifndef DEFAULTSETTINGS_HPP
# define DEFAULTSETTINGS_HPP
#include <cstddef>
#include <map>
#include <string>
#include "../Parsing/NginxReturn.hpp"
#include <vector>
#include "../Parsing/Directive.hpp"

#define ROOT_FOLDER "www/"
#define DEFAULT_LISTEN_PORT 8080
#define DEFAULT_SERVER_NAME "localhost"
#define DEFAULT_MAX_BODY_SIZE 1000000
#define DEFAULT_UPLOAD "uploads"

class DefaultSettings 
{
	public :
		const bool&						getFirstListenApplyFlag() const;
		void							setListenFlagFalse();
		const bool&						getFirstNameApply() const;
		void 							setNameFlagFalse();
		bool 							isContainingName(const std::string& nameToCheck) const;
		bool							isListeningToPort(const int& portToCheck) const;
		static void						checkDuplicateDirectives(const std::vector<Directive>& dirVec);
		bool							isMethodAllowed(std::string method) const;
		void 							addListenPort(const int& listenPort);
		void 							setErrorPage(int errorCode, std::string path);
		void 							setAllAcceptedMethodsToFalse(void);
		void 							setAcceptedMethodToTrue(std::string methodName);
		void 							setClientMaxBodySize(size_t maxSize);
		void 							setAutoIndex(bool state);
		void 							setNginxReturn(int statusCode, std::string redirectPath, bool flag = 1);
		void 							setIndexes(std::vector<std::string> indexes);
		void 							setRoot(std::string root);
		void 							addServerName(const std::string& serverName);
		void 							setCgiExtensions(std::vector<std::string> extensionsVector);
		void 							setUploadFolder(const std::string& folderName);
		const std::vector<int>&			getPorts(void) const;
		void							removeDefaultListen();
		void							removeDefaultName();
		const std::string& 				getRoot(void) const;
		const std::vector<std::string>&	getServerName(void) const;
		const NginnxReturn&				getNginxReturn(void) const;
		const bool& 					getAutoindexFlag(void) const;
		const std::vector<std::string>&	getIndexes() const;
		const std::string&				getUploadFolder(void) const;
		void							checkDefaultFolderAcces() const;
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
		std::vector<std::string>	p_serverName; // This is host
		bool						p_firstNameApply;
		std::vector<int> 			p_listenPort;
		bool						p_firstListenApply;


		//location stuff
		std::map<int, std::string>	p_errorPages;
		std::map<std::string, bool>	p_acceptedMethods;// limitExcept
		std::size_t					p_clientMaxBody;
		bool 						p_autoindex;
		NginnxReturn 				p_return;
		std::vector<std::string>	p_index;
		std::string					p_root;
		std::vector<std::string>	p_cgiExtensions;
		std::string					p_uploadFolder;
		
	private :
		void						_setDefaultHttpMethods(void);
		void						_setDefaultIndexes(void);


	public:
		friend std::ostream& operator<<(std::ostream& os, const DefaultSettings& server);
};


#endif
