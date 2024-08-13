
# include <iostream>
# include <vector>
# include <cstring>

char**	vec_to_chararr(std::vector<std::string> list)
{
	int i = 0;
	std::vector<std::string>::iterator it = list.begin();
	char**	_tmp = new char*[list.size() + 1];

	for (; it != list.end(); it++)
	{
		char* line = new char[it->size() + 1];
		for (size_t j = 0; j < it->size(); j++)
			line[j] = it->c_str()[j];
		_tmp[i] = line;
		_tmp[i][it->size()] = '\0';
		i++;
	}
	_tmp[i] = NULL;
	char** result = _tmp;
	_tmp = NULL;
	return (result);
}

void	delete_chararr(char ** lines)
{
	if (!lines)
		return ;
	size_t i = 0;
	for(; lines[i]; i++)
		delete [] lines[i];
	delete [] lines[i];
	delete [] lines;
}

int main()
{
	std::vector<std::string> vec = {"whats", "happening", "!"};
	char ** lines = vec_to_chararr(vec);

	for(size_t i = 0; lines[i]; i++)
		std::cout << lines[i] << std::endl;
	delete_chararr(lines);
}
