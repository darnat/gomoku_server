SRC			= $(filter-out %~, $(shell ls sources))

CFLAGS			= -W -Werror -Wall -Wextra -pedantic -g -I headers -I gomoku_ai/headers

OBJ			= $(addprefix objects/,$(SRC:.c=.o))

CC			= gcc

NAME			= gomoku

all			: $(NAME)

$(NAME)			: $(OBJ)
			$(CC) $(OBJ) -o $(NAME)

objects/%.o		: sources/%.c
			@mkdir -p objects
			$(CC) $(CFLAGS) -c $< -o $@

clean			:
			rm -f $(OBJ)

fclean			: clean
			rm -f $(NAME)

re			: fclean all

.PHONY			: all clean fclean re
