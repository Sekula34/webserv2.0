#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP
#include "DefaultSettings.hpp"
#include "Token.hpp"
#include <cstddef>
#include <exception>
#include <string>
#include <vector>

class Directive 
{
	private :

		static const std::string _validHttpDirectives[];
		static const std::string _validServerDirectives[];
		static const std::string _validLocationDirectives[];
		static const std::string _validHttpMethods[];
		std::string _directiveName;
		std::string _directiveValue;

		std::vector<Token> _dirPath;
		std::size_t _dirLineNumber;
		std::string _getNameFromToken(const Token& token) const;
		std::string _getValueFromToken(const Token& token) const;
		bool _isNameValid(const std::string& name, const std::string validList[],
						ContextType context) const;

		int _stringToInt(std::string stringValue) const;
		

		void _applyErrorPage(DefaultSettings& settings);
		void _applyListen(DefaultSettings& settings);
		void _applyLimitExcept(DefaultSettings& settings);
		void _applyClientMaxBodySize(DefaultSettings& settings);
		void _apllyAutoIndex(DefaultSettings& settings);
		void _applyReturn(DefaultSettings& settings);
		void _applyIndex(DefaultSettings& settings);
		void _apllyRoot(DefaultSettings& settings);
		void _applyServerName(DefaultSettings& settings);

	public :
		Directive();
		Directive(std::string dirName, std::string dirValue);
		Directive(const Token& token);
		Directive(const Directive& source);
		Directive& operator=(const Directive& source);
		~Directive();

		size_t getDirectivePathSize(void) const;
		void apply(DefaultSettings& settings);
		void printDirectiveInfor(void) const;
		static void printAllDirectives(const std::vector<Directive>& allDirectives);
		static std::vector<Directive> getAllServerDirectives(const std::vector<Token>& allServerTokens);
		static void applyAllDirectives(std::vector<Directive>& allDirectives, DefaultSettings& settings);
		class InvalidDirectiveException : public std::exception 
		{
			const char * what() const throw();
		};
};

#endif 
