NAME = webserv

SRC = \
	srcs/main.cpp \
	srcs/parser/Parser.cpp \
	srcs/parser/config/AConfig.cpp \
	srcs/parser/config/GlobalConfig.cpp \
	srcs/parser/config/LocationConfig.cpp \
	srcs/parser/config/ServerConfig.cpp \
	srcs/lexer/Lexer.cpp \

OBJ = $(SRC:.cpp=.o)
DEPS = $(SRC:.cpp=.d)

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
# CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -MMD -MD

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
-include $(DEPS)

clean:
	rm -f $(OBJ) $(DEPS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re