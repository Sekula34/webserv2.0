#ifndef HTTPSTATUSCODE_HPP
# define HTTPSTATUSCODE_HPP
#include <exception>
#include <map>
#include <string>

/**
 * @brief static class for getting ReasonPhrase.
 * example 404 will return Not Found. 200 will return OK
 * 
 */
class HttpStatusCode 
{
	private : 
		static std::map<int, std::string> _getFilledMap();
		static std::map<int, std::string> _statusCode;

	public:
		/**
		 * @brief Get the Reason Phrase message behind code
		 * 
		 * @param code code that you want to turn in Reason
		 * @throw UnkownCodeException if cannot find code in static map _status code
		 * @return std::string 
		 */
		static std::string getReasonPhrase(int code);

		class UnkownCodeException : public std::exception
		{
			public :
				const char * what() const throw();
		};
};

#endif
