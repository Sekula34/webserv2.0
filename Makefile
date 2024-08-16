#------------------------------- EXECUTABLE ---------------------------------#
NAME =		webserv

#--------------------------------- COMPILER -----------------------------------#
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g
# CC = clang-12
# CFLAGS = -Wall -Wextra -Werror -gdwarf-4 -g3
# COMPILER_FLAGS = -Wall -Wextra -Werror -g -fsanitize=address,leak,undefined
# LIBS = -lasan -lubsan -lreadline

#------------------------------- ABBREVIATIONS --------------------------------#
RM = rm -rf
MKDIR = mkdir -p
ARCHIVE = ar rcs

#-------------------------------- ANSI COLORS ---------------------------------#
BLACK := \033[0;30m
RED := \033[0;31m
GREEN := \033[0;32m
YELLOW := \033[0;33m
BLUE := \033[0;34m
PURPLE := \033[0;35m
CYAN := \033[0;36m
WHITE := \033[0;37m
NC := \033[0m # No color

MAIN=srcs/main.cpp

SRCS =		srcs/Client/ClientHeader.cpp\
			srcs/epoll/Client.cpp \
			srcs/epoll/CgiProcessor.cpp \
			srcs/Parsing/LocationSettings.cpp \
			srcs/Parsing/ServersInfo.cpp \
			srcs/Parsing/Token.cpp \
			srcs/Parsing/DefaultSettings.cpp \
			srcs/Parsing/NginxReturn.cpp \
			srcs/Parsing/Directive.cpp \
			srcs/Parsing/Configuration.cpp \
			srcs/Parsing/ParsingUtils.cpp \
			srcs/Parsing/ServerSettings.cpp \
			srcs/Response/Response.cpp\
			srcs/Response/ResponseBody.cpp\
			srcs/Response/ResponseHeader.cpp\
			srcs/Server/EpollHandler.cpp \
			srcs/Server/ConnectionDispatcher.cpp \
			srcs/Server/Socket.cpp \
			srcs/Server/SocketManager.cpp \
			srcs/Utils/FileUtils.cpp\
			srcs/Utils/HttpStatusCode.cpp\
			srcs/Utils/Logger.cpp\
			srcs/Utils/UrlSuffix.cpp\
			testers/ServerTest.cpp\
			testers/UnitTest.cpp \
			testers/CHTest.cpp
			# $(MAIN)\


HEADERS =	srcs/Client/ClientHeader.hpp\
			srcs/epoll/Client.hpp\
			srcs/epoll/CgiProcessor.hpp\
			srcs/Parsing/LocationSettings.hpp\
			srcs/Parsing/ServersInfo.hpp\
			srcs/Parsing/Token.hpp\
			srcs/Parsing/DefaultSettings.hpp\
			srcs/Parsing/NginxReturn.hpp\
			srcs/Parsing/Directive.hpp\
			srcs/Parsing/Configuration.hpp\
			srcs/Parsing/ParsingUtils.hpp\
			srcs/Parsing/ServerSettings.hpp\
			srcs/Response/Response.hpp\
			srcs/Response/ResponseBody.hpp\
			srcs/Response/ResponseHeader.hpp\
			srcs/Server/EpollHandler.hpp \
			srcs/Server/ConnectionDispatcher.hpp\
			srcs/Server/Socket.hpp\
			srcs/Server/SocketManager.hpp\
			srcs/Utils/FileUtils.hpp\
			srcs/Utils/HttpStatusCode.hpp\
			srcs/Utils/Logger.hpp\
			srcs/Utils/UrlSuffix.hpp\
			testers/ServerTest.hpp\
			testers/UnitTest.hpp\
			testers/CHTest.hpp
			

OBJ_DIR = obj

# OBJ = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))
OBJ = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

OBJ_DIRS := $(sort $(dir $(OBJ)))

#------------------------------------ CODE ------------------------------------#
.PHONY: all clean fclean re bonus generate_test test

all: $(NAME)


$(NAME): $(OBJ) $(HEADERS) srcs/main.o  
	@printf "\r\033[K"
	@printf "$(YELLOW)$(NAME)!\n"
	@$(MAKE) --no-print-directory mainandtest
	@$(CXX) $(OBJ) obj/srcs/main.o -o $(NAME) 
	@printf "$(GREEN)$(NAME) created.$(NC)\n"
	@printf "$(GREEN)$(NAME) done$(NC)\n"

$(OBJ_DIR)/%.o: %.cpp $(HEADERS) | $(OBJ_DIRS) 
	@printf "\r\033[K"
	$(LOADING_BAR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

mainandtest: | $(OBJ_DIRS) 
	@$(CXX) $(CXXFLAGS) -c srcs/main.cpp -o  obj/srcs/main.o
	@$(CXX) $(CXXFLAGS) -c testers/unitTestmain.cpp -o obj/testers/unitTestmain.o

$(OBJ_DIRS):
	@mkdir -p $@

clean:
	@rm -rf $(OBJ_DIR)
	@printf "$(RED)Deleted objects$(NC)\n"

fclean: clean
	@rm -rf $(NAME) 
	@rm -rf unittest 
	@printf "$(RED)Deleted $(NAME)$(NC)\n"

re:
	@printf "$(YELLOW)Rebuilding...$(NC)\n"
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all

bonus: all


valgrind:		
					valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)
		

unitTest : $(OBJ) $(HEADERS) testers/unitTestmain.cpp 
			@$(MAKE) --no-print-directory mainandtest
			@$(CXX) $(CXXFLAGS) $(OBJ) obj/testers/unitTestmain.o -o unittest
			@printf "$(GREEN)unitTest created.$(NC)\n"


.PHONY:			all clean fclean re valgrind run conf confclean confre

#-------------------------------- LOADING BAR ---------------------------------#
COUNT = 1
TOTAL_FILES := $(words $(SRCS))

define LOADING_BAR
	@PERCENT=$$(( $$(($(COUNT) * 100 / $(TOTAL_FILES))) )); \
	if [ $$PERCENT -le 25 ]; then \
		COLOR="$(YELLOW)"; \
	elif [ $$PERCENT -le 50 ]; then \
		COLOR="$(GREEN)"; \
	elif [ $$PERCENT -le 75 ]; then \
		COLOR="$(CYAN)"; \
	else \
		COLOR="$(PURPLE)"; \
	fi; \
	FILLED=$$(( $$PERCENT / 5 )); \
	UNFILLED=$$(( 20 - $$FILLED )); \
	BAR=""; \
	for i in $$(seq 1 $$FILLED); do BAR="$$BAR#"; done; \
	for i in $$(seq 1 $$UNFILLED); do BAR="$$BAR-"; done; \
	printf "[$$COLOR$$BAR$(WHITE)] $(CYAN)Compiling $$PERCENT%%$(NC) - $<\r"; \
	$(eval COUNT=$(shell expr $$(($(COUNT) + 1))))
endef
