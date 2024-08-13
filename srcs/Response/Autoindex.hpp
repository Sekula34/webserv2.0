#ifndef AUTOINDEX
# define AUTOINDEX
#include <string>


class Autoindex
{
	public :
		Autoindex(const std::string folderPath, int& statusCode);
		Autoindex(const Autoindex& source);
		Autoindex& operator=(const Autoindex& source);
		~Autoindex();

		int& getStatusCode() const;

	private:
		Autoindex();
		const std::string _folderPath;
		int& _statusCode;
};

#endif
