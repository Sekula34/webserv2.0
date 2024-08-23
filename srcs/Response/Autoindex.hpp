#ifndef AUTOINDEX
# define AUTOINDEX
#include <string>
#include <vector>

class AutoindexTest;

class Autoindex
{
	public :
		Autoindex(const std::string folderPath, int& statusCode);
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
		void _createHtmlMenu();
		void _createAndStoreOneFileLink(const std::string fileEntry);
		void _generateHtmlPage(const std::string pageTitle);
	friend AutoindexTest;
};

#endif
