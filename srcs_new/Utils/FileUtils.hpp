#ifndef FILEUTILS_HPP
# define FILEUTILS_HPP
#include <string>

class FileUtils
{
	public :
		enum e_fileType
		{
			HTML, 
			CSS,
			TXT,
			UNKOWN 
		};

		static void setConfigFilePath(const std::string &configFilePath);
		static const std::string& getConfigFilePath(); 
		static bool isPathValid(const std::string relativeFilePath);
		/**
		 * @brief put file in string 
		 * 
		 * @param relativeFilePath to the file, (Relative to executable webserv), ./ will be appended by this function
		 * @param string in which file will be putted
		 * @return true file is regular and it is succesfully in string
		 * @return false either file is not regular, not permission, or something failed, generate 500 http code in that case
		 */
		static bool putFileInString(const std::string relativeFilePath, std::string& string);

		static bool isDirectoryValid(const std::string relativeDirPath, int& httpStatusCode);

		/**
		 * @brief check if filepath relative to executable is file or folder
		 * 
		 * @param serverFilePath 
		 * @return int 1 File, 2-Folder, -1 fail (Internal server), 0 Only God knows
		 */
		static int isPathFileOrFolder(const std::string& serverFilePath, int& httpStatusCode);
		static bool	isValidName(const std::string& fileName);

	private:
		static std::string _configFilePath;
		static void _setDirFailStatusCode(int ernnoNum, int& httpStatusCode);
		static void _setFileOrFolderStatusCode(int errnoNum, int& httpStatusCode);
};

#endif
