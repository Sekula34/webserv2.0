#ifndef TOKEN_HPP
# define TOKEN_HPP

#include <cstddef>
#include <exception>
#include <string>
#include <vector>

const std::string yellow= "\033[1;33m";
const std::string resetText = "\033[0m";

const std::string validContexts[] = {"http", "server", "location"};



class Token
{
	public :
		enum TokenType
		{
			UNKOWN,
			OPENING_BRACE,
			CLOSING_BRACE,
			CONTEXT,
			DIRECTIVE
		};

		enum ContextType
		{
			HTTP,
			SERVER,
			LOCATION,
			NOT_CONTEXT
		};

		static void					checkAllTokensContext(std::vector<Token>& allTokens);
		static void					printAllTokensInfo(const std::vector<Token>& allTokens);
		static std::vector<Token>	getAllServerTokens(int serverId, std::vector<Token>& allTokens);
		std::string					getTokenInfo() const;
		size_t						getTokenLineNumber() const;
		TokenType					getTokenType(void) const;
		ContextType					getTokenContextType(void) const;
		int							getTokenServerId(void) const;
		void						printTokenInfo(void) const;
		std::vector<Token>			getTokenPath(void) const;
		void						setTokenPath(std::vector<Token> path);
		void						setTokenType(TokenType newType);
		ContextType					getCurrentTokenContextType() const;
		
									Token();
									Token(const std::string &info, size_t lineNumber = 0, int serverId = -1);
									Token(const Token& source);
		Token&						operator=(const Token& source);
									~Token();
	private :
		std::string			_info;
		TokenType			_type;
		std::size_t			_lineNumber;
		int					_serverId;
		std::vector<Token>	_path;
		ContextType			_contextType;

		void				_contextChecker(void);
		void 				_checkOneToken(void);

	public:
		class InvalidTokenException : public std::exception
		{
			const char * what() const throw();
		};
		
};

#endif
