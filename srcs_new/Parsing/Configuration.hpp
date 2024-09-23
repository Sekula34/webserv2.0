#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "Token.hpp"
# include <cstddef>
# include <exception>
# include <string>
# include <vector>
# include <utility>

// const std::string yellow= "\033[1;33m";
// const std::string resetText = "\033[0m";

typedef std::pair<std::string, size_t > linePair ;

class Configuration 
{
	public :
		const std::string&		getFilePath() const;
		void					printFileMember(void);
		std::vector<Token>		getAllTokens() const;
		int						getNumberOfServers(void) const;

								Configuration(const std::string& filePath);
								Configuration(const Configuration& source);
		Configuration&			operator=(const Configuration& source);
								~Configuration();

	private :
	 	int						_serverId;
		const std::string		_filePath;
		std::vector<linePair>	_fileLine;
		std::string 			_confOneLine;
		std::vector<Token> 		_tokensVector;

		void 					_checkFileAccessType(void) const;
		/**
		 * @brief function that check if http Token is trying to be added in vector not on 0 index
		 * 
		 * @param oneToken 
		 * @throw  InvalidConfig file if token info is http and size of _tokens vector is not 0 which means there is one http too much
		 */
		void					_checkDoubleHttp(const Token& oneToken);
		void					_copyFileInVector(void);
		void					_createOneToken(std::string& tokeInfo, size_t lineNumber);
		bool					_isCharDelimiter(char c) const;
		void					_fillAllTokensPaths(void);
		void					_fillTokensVector(void);
		std::string				_generateServerIdString(int serverId);
		std::string				_getCleanConfLine(const std::string& dirtyLine);
		void					_removeSpacesAndTabs(std::string& dirtyLine);
		
	public: 
		class InvalidConfigFileException : public std::exception
		{
			const char* what() const throw();
		};
};

#endif
