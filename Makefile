#------------------------------- EXECUTABLE ---------------------------------#
NAME =		webserv

#--------------------------------- COMPILER -----------------------------------#
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98
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


SRCS =	srcs_new/Client/Client.cpp \
	srcs_new/Cgi/Cgi.cpp \
	srcs_new/Client/FdData.cpp \
	srcs_new/Io/Io.cpp \
	srcs_new/Message/Node.cpp \
	srcs_new/Message/RequestHeader.cpp \
	srcs_new/Message/ResponseHeader.cpp \
	srcs_new/Message/AHeader.cpp \
	srcs_new/Message/Message.cpp \
	srcs_new/Parsing/ParsingUtils.cpp \
	srcs_new/Parsing/Directive.cpp \
	srcs_new/Parsing/NginxReturn.cpp \
	srcs_new/Parsing/Token.cpp \
	srcs_new/Parsing/Configuration.cpp \
	srcs_new/Server/Socket.cpp \
	srcs_new/Server/ConnectionManager.cpp \
	srcs_new/Server/ResponseGenerator.cpp \
	srcs_new/Server/ServerManager.cpp \
	srcs_new/Server/VirtualServer.cpp \
	srcs_new/Server/DefaultSettings.cpp \
	srcs_new/Server/LocationSettings.cpp \
	srcs_new/Utils/FileUtils.cpp \
	srcs_new/Utils/HttpStatusCode.cpp \
	srcs_new/Utils/UrlSuffix.cpp \
	srcs_new/Utils/Data.cpp \
	srcs_new/Utils/Logger.cpp \


HEADERS =	srcs_new/Client/Client.hpp \
	srcs_new/Cgi/Cgi.hpp \
	srcs_new/Client/FdData.hpp \
	srcs_new/Io/Io.hpp \
	srcs_new/Message/Node.hpp \
	srcs_new/Message/RequestHeader.hpp \
	srcs_new/Message/ResponseHeader.hpp \
	srcs_new/Message/AHeader.hpp \
	srcs_new/Message/Message.hpp \
	srcs_new/Parsing/ParsingUtils.hpp \
	srcs_new/Parsing/Directive.hpp \
	srcs_new/Parsing/NginxReturn.hpp \
	srcs_new/Parsing/Token.hpp \
	srcs_new/Parsing/Configuration.hpp \
	srcs_new/Server/Socket.hpp \
	srcs_new/Server/ConnectionManager.hpp \
	srcs_new/Server/ResponseGenerator.cpp \
	srcs_new/Server/ServerManager.hpp \
	srcs_new/Server/VirtualServer.hpp \
	srcs_new/Server/DefaultSettings.hpp \
	srcs_new/Server/LocationSettings.hpp \
	srcs_new/Utils/FileUtils.hpp \
	srcs_new/Utils/HttpStatusCode.hpp \
	srcs_new/Utils/UrlSuffix.hpp \
	srcs_new/Utils/Data.hpp \
	srcs_new/Utils/Logger.hpp 
			

OBJ_DIR = obj

# OBJ = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))
OBJ = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

OBJ_DIRS := $(sort $(dir $(OBJ)))

#------------------------------------ CODE ------------------------------------#
.PHONY: all clean fclean re bonus generate_test test unitTest

all: $(NAME)


$(NAME): $(OBJ) $(HEADERS) srcs_new/main.cpp
	@printf "\r\033[K"
	@printf "$(YELLOW)$(NAME)!\n"
	 @$(MAKE) --no-print-directory mainandtest
	@$(CXX) $(OBJ) obj/srcs_new/main.o -o $(NAME) 
	@printf "$(GREEN)$(NAME) created.$(NC)\n"
	@printf "$(GREEN)$(NAME) done$(NC)\n"

$(OBJ_DIR)/%.o: %.cpp $(HEADERS) | $(OBJ_DIRS) 
	@printf "\r\033[K"
	$(LOADING_BAR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

mainandtest: | $(OBJ_DIRS) 
	@$(CXX) $(CXXFLAGS) -c srcs_new/main.cpp -o  obj/srcs_new/main.o
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
					valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes ./$(NAME)
					#valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --track-origins=yes --trace-children=yes ./$(NAME)
		

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
