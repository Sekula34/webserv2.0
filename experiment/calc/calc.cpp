# include <iostream>
# include <sstream>

int	main()
{
	unsigned int x;   
	std::stringstream ss;
	ss << std::hex << "E6;asdf;af;fuck\r\n\r\n";
	ss >> x;


	std::cout << x << std::endl;
}
