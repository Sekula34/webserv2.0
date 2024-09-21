#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

#include "Token.hpp"
#include <cstddef>
#include <exception>
#include <string>
#include <vector>

class DirectiveTester;
class DefaultSettings;

class Directive 
{
	friend DirectiveTester;
	private :

		static const std::string _validHttpDirectives[];
		static const std::string _validServerDirectives[];
		static const std::string _validLocationDirectives[];
		static const std::string _validHttpMethods[];
		static const std::string _uniqueDirectives[];
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
		void _applyCgiExtension(DefaultSettings& settings);
		// Functor
		class FindbyDirectiveName
		{
			private : 
				std::string _directiveName;
			public: 
				FindbyDirectiveName(const std::string& directiveName)
				:_directiveName(directiveName) {}

				bool operator()(const Directive& directive)
				{
					return (directive._directiveName == _directiveName);
				}
		};


	public :
		Directive();
		Directive(std::string dirName, std::string dirValue);
		Directive(const Token& token);
		Directive(const Directive& source);
		Directive& operator=(const Directive& source);
		~Directive();


		const std::string& getDirectiveName(void) const;
		const size_t& getDirectiveLineNum(void) const;
		size_t getDirectivePathSize(void) const;
		void apply(DefaultSettings& settings);
		void printDirectiveInfor(void) const;
		static void printAllDirectives(const std::vector<Directive>& allDirectives);
		static std::vector<Directive> getAllServerDirectives(const std::vector<Token>& allServerTokens);
		static void applyAllDirectives(std::vector<Directive>& allDirectives, DefaultSettings& settings);

		/**
		 * @brief Function that goes through vector of directives and check if list contains duplicates that are not allowed.
		 For example autoindex root .. can be defined only once in config file. 
		 * 
		 * @param directives vector that contains all directives
		 * @param duplicateDir referece to a pointer to const Directive. Directive object cannot be modifed through this pointer.
		 & means that function can modify the pointer itself but not the object. There will be stored all data of first duplicate in vector if(true); 
		 * @return true duplicate directive is presen
		 * @return false ther eis no duplicates that are not allowed
		 */
		static bool isDuplicateDirectivePresent(const std::vector<Directive>& directives, const Directive* &duplicateDir);
		
		static bool isDuplicateDirectiveNamePresent(const std::vector<Directive>& directives, const Directive* &duplicateDir, const std::string& nameToCheck);
		
		class InvalidDirectiveException : public std::exception 
		{
			const char * what() const throw();
		};
};

#endif 
