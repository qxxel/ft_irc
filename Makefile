NAME = ircserv

DIR_OBJS = .obj

INCLUDE_DIR = includes

FLAGS = -Wall -Werror -Wextra -std=c++98 -g3

DFLAGS = -MD -MP

SRC = 	main.cpp Server.cpp Client.cpp Request.cpp Command.cpp Channel.cpp signals.cpp Bot.cpp

SRC_PATH = srcs/

CC = c++

CHECK = ✅
RED = $(shell tput setaf 1)
CROSS = ❌
GREEN = $(shell tput setaf 2)
RESET_COLOR = $(shell tput sgr0)
CYAN = $(shell tput setaf 6)

OBJ = $(addprefix $(DIR_OBJS)/, $(SRC:.cpp=.o))
DEP = $(OBJ:.o=.d)

all:$(NAME)

IFLAGS = -I$(INCLUDE_DIR)

-include $(DEP)

$(DIR_OBJS)/%.o: $(SRC_PATH)%.cpp Makefile
	@mkdir -p  $(dir $@)
	@$(CC)  $(IFLAGS) $(FLAGS) $(DFLAGS) -c $< -o $@
	@echo "$(CYAN) $< created $(CHECK)$(RESET_COLOR)"

$(NAME): $(OBJ)
	@$(CC) $(OBJ) -o $(NAME)
	@echo "$(GREEN)$(NAME) created $(CHECK)$(RESET_COLOR) "

clean:
	@rm -rf $(DIR_OBJS)
	@echo "$(RED)$(DIR_OBJS) removed $(CROSS)$(RESET_COLOR) "

fclean: clean
	@rm -rf $(NAME)
	@echo "$(RED)$(NAME) removed $(CROSS)$(RESET_COLOR) "

FORCE:

re: fclean all

.PHONY: all re clean fclean re FORCE
