#ifndef AUTOINDEX
# define AUTOINDEX
#include <string>
#include <vector>
#include <dirent.h>


class AutoindexTest;

class Autoindex
{
	public :
		Autoindex(const std::string folderPath, int& statusCode, const std::string linkBase = "");
		Autoindex(const Autoindex& source);
		Autoindex& operator=(const Autoindex& source);
		~Autoindex();

		int& getStatusCode() const;
		const std::string& getAutoIndexHtml() const;

	private:
		Autoindex();
		const std::string _folderPath;
		std::vector<std::string> _fileLinks;
		std::string _autoIndexHtml;
		int& _statusCode;
		const std::string _linkBase;
		void _createHtmlMenu();
		void _createDirectoryLinks(DIR* directory);
		void _createAndStoreOneFileLink(const std::string fileEntry);
		void _generateHtmlPage(const std::string pageTitle);

	//MR_NOTE: Be careful with this one! I think we cannot use friend.
	friend AutoindexTest;
};

#endif
