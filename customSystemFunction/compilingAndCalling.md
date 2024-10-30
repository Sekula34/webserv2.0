## compiling
`c++ -shared -fPIC custom_accept.cpp -ldl -o custom_accept.so`
## Runing
`LD_PRELOAD=./custom_accept.so ./webserv`

## leak
`LD_PRELOAD=./custom_accept.so valgrind --leak-check=full --show-leak-kinds=all ./webserv` 
