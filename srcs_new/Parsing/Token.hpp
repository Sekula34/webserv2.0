#ifndef TOKEN_HPP
# define TOKEN_HPP

#include <cstddef>
#include <exception>
#include <ostream>
#include <string>
#include <vector>


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
		static std::vector<Token>	getAllServerTokens(int serverId, std::vector<Token>& allTokens);
		const std::string&			getTokenInfo() const;
		const size_t&				getTokenLineNumber() const;
		const TokenType&			getTokenType(void) const;
		const ContextType&			getTokenContextType(void) const;
		const int&					getTokenServerId(void) const;
		const std::vector<Token>&	getTokenPath(void) const;
		void						setTokenPath(std::vector<Token> path);
		void						setTokenType(TokenType newType);
		const ContextType&			getCurrentTokenContextType() const;

									Token();
									Token(const std::string &info, size_t lineNumber = 0, int serverId = -1);
									Token(const Token& source);
		Token&						operator=(const Token& source);
									~Token();
	private :
		static const std::string	_validContexts[];
		std::string					_info;
		TokenType					_type;
		std::size_t					_lineNumber;
		int							_serverId;
		std::vector<Token>			_path;
		ContextType					_contextType;

		void						_contextChecker(void);
		void 						_checkOneToken(void);

	public:
		class InvalidTokenException : public std::exception
		{
			const char * what() const throw();
		};
		friend std::ostream& operator<<(std::ostream& os, const Token& token);
		
};

#endif
