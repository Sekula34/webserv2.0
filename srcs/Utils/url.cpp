#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

#define VALID_URI_CHARS "-_.~"

bool	ft_isValidURIChar(unsigned char& c)
{
	static std::string validURIChars(VALID_URI_CHARS);
	return (isalnum(c) || validURIChars.find(c) != std::string::npos);
}

std::string	uriEncode(bool encodeSpaceAsPlus, const std::string& input)
{
	std::ostringstream encoded;
	for (size_t i = 0; i < input.length(); ++i)
	{
		unsigned char c = input[i];
		// 1st check mode and space for faster performance.
		if (encodeSpaceAsPlus && c == ' ')
			encoded << '+';
		// Check if the character is safe (if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~'))
		else if (ft_isValidURIChar(c))
			encoded << c;
		else
		{
			// Convert character to its hex representation
			encoded << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << int(c);
		}
	}
	return (encoded.str());
}

std::string	uriDecode(bool decodePlusAsSpace, const std::string& input)
{
	std::ostringstream decoded;
	for (size_t i = 0; i < input.length(); ++i)
	{

		// 1st check mode and + for faster performance.
		if (decodePlusAsSpace && input[i] == '+')
			decoded << ' ';
		else if (input[i] == '%' && i + 2 < input.length())
		{
			// Convert hex to char
			std::istringstream hexStream(input.substr(i + 1, 2));
			int hexValue;
			hexStream >> std::hex >> hexValue;
			decoded << static_cast<char>(hexValue);
			i += 2; // Skip over the hex digits
		}
		else
			decoded << input[i];
	}
	return (decoded.str());
}

// int	main(int argc, char** argv)
// {
// 	if (argc != 3)
// 	{
// 		std::cout << "Usage: " << argv[0] << " + mode (URL encode / URL decode) + 'string to convert'" << std::endl;
// 		return (1);
// 	}
// 	std::string mode(argv[1]);
// 	if (mode == "URL encode")
// 		std::cout << uriEncode(false, argv[2]) << std::endl;
// 	else if (mode == "URL decode")
// 		std::cout << uriDecode(false, argv[2]) << std::endl;
// 	return (0);
// }
