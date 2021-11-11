GCC = gcc

SRCS = ./src/read_from_file.c ./src/RB_tree.c ./src/ft_split.c

NAME = prog

FLAGS = -W -Wall -Werror

all : $(NAME)

$(NAME) : $(SRCS)
	$(GCC) $(FLAGS) $(SRCS) -o $(NAME)

test : 
	./$(NAME) input.txt
	-diff my_output.txt output.txt > difference.txt

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY : all clean fclean re
