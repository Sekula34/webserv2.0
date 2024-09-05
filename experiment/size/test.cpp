#include <iostream>
#include <cmath>

int	main()
{
	int	bodySize = 31 ;
	int	max_chunksize = 10;

	int ceiling = std::ceil(static_cast<double>(bodySize) / static_cast<double>(max_chunksize));
	std::cout << "Max Chunksize: " << ceiling << std::endl;
}
