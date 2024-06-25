GREEN =		\033[0;32m
RED =		\033[0;31m
YELLOW =	\033[0;33m
RESET =		\033[0m

NAME =		webserv

CONFNAME = conf.out


PARSING_SRCS = srcs/Parsing/LocationSettings.cpp \
			srcs/Parsing/ServersInfo.cpp \
			srcs/Parsing/Token.cpp \
			srcs/Parsing/DefaultSettings.cpp \
			srcs/Parsing/NginxReturn.cpp \
			srcs/Parsing/Directive.cpp \
			srcs/Parsing/Configuration.cpp \
			srcs/Parsing/ParsingUtils.cpp \
			srcs/Parsing/ServerSettings.cpp \

PARSING_OBJS = $(patsubst srcs/%.cpp, ParsingObjects/%.o, $(PARSING_SRCS))

ParsingObjects/%.o: srcs/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

SRCS =		srcs/Client/ClientRequest.cpp\
			srcs/Client/ClientHeader.cpp \
			srcs/Parsing/LocationSettings.cpp \
			srcs/Parsing/ServersInfo.cpp \
			srcs/Parsing/Token.cpp \
			srcs/Parsing/DefaultSettings.cpp \
			srcs/Parsing/NginxReturn.cpp \
			srcs/Parsing/Directive.cpp \
			srcs/Parsing/Configuration.cpp \
			srcs/Parsing/ParsingUtils.cpp \
			srcs/Parsing/ServerSettings.cpp \
			srcs/Server/ConnectionDispatcher.cpp \
			srcs/Server/Socket.cpp \
			srcs/Server/SocketManager.cpp \
			#srcs/main.cpp


HEADERS =	srcs/Client/ClientRequest.hpp\
			srcs/Client/ClientHeader.hpp \
			srcs/Parsing/LocationSettings.hpp \
			srcs/Parsing/ServersInfo.hpp \
			srcs/Parsing/Token.hpp \
			srcs/Parsing/DefaultSettings.hpp \
			srcs/Parsing/NginxReturn.hpp \
			srcs/Parsing/Directive.hpp \
			srcs/Parsing/Configuration.hpp \
			srcs/Parsing/ParsingUtils.hpp \
			srcs/Parsing/ServerSettings.hpp \
			srcs/Server/ConnectionDispatcher.hpp \
			srcs/Server/Socket.hpp \
			srcs/Server/SocketManager.hpp \

OBJDIR =	obj
OBJS =		$(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))

CXX =		c++

CXXFLAGS =	-g -std=c++98 -Wall -Werror -Wextra -pedantic

RM =		rm -fr

all:			$(NAME)

$(NAME):		$(OBJS) $(HEADERS) srcs/main.cpp 
					@$(CXX) $(OBJS) $(CXXFLAGS) srcs/main.cpp -o $(NAME)
					@echo "$(GREEN)Compilation successful.$(RESET) Executable name: ./$(NAME)"

$(OBJDIR)/%.o:	%.cpp $(HEADERS)
					@mkdir -p $(dir $@)
					@$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
					@$(RM) $(OBJS)
					@$(RM) $(OBJDIR)

fclean:			clean
					@$(RM) $(NAME)

re:				fclean all

valgrind:		
					valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all ./$(NAME)
		
conf: $(CONFNAME)

confclean :
	rm -rf $(PARSING_OBJS) $(CONFNAME) 
	rm -rf ParsingObjects

confre: 
	$(MAKE) confclean 
	$(MAKE) conf

$(CONFNAME) : $(PARSING_OBJS) $(HEADERS)
	$(CXX) $(PARSING_OBJS) $(CXXFLAGS)  Configuration/main.cpp -o $(CONFNAME)

.PHONY:			all clean fclean re valgrind run conf confclean confre
