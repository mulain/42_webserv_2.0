NAME	=	webserv

CC		=	c++
RM		=	rm -rf
FLAGS	=	-Wall -Wextra -Werror -pedantic -std=c++98 #-fsanitize=address
INCLUDE	=	-Iincl -Iincl/exceptions -Iincl/Response -Iincl/setup

COLOR	=	\033[30m
RESET	=	\033[0m

SRCFILE	=	main.cpp\
			$(addprefix exceptions/, ErrorCode.exception.cpp CloseConnection.exception.cpp)\
			$(addprefix Response/, A_Response.cpp DynContent.Response.cpp File.Response.cpp)\
			$(addprefix setup/, Config.cpp ConfigFile.cpp)\
			$(addprefix utils/, utils_file.cpp utils_misc.cpp utils_string.cpp)\
			Binding.cpp\
			Request.cpp\
			Client.cpp\
			Server.cpp
SRC		=	$(addprefix src/, $(SRCFILE))
OBJ		= 	$(addprefix obj/, $(SRCFILE:%.cpp=%.o))

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(INCLUDE) $(OBJ) -o $(NAME) 
	@echo "$(COLOR)$(NAME) compiled.$(RESET)"

obj/%.o: src/%.cpp
	@mkdir -p obj/exceptions
	@mkdir -p obj/Response
	@mkdir -p obj/setup
	@mkdir -p obj/utils
	$(CC) $(FLAGS) $(INCLUDE) -c $< -o $@

clean:
	@$(RM) $(OBJ) obj
	@echo "$(COLOR)$(NAME) cleaned.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(COLOR)$(NAME) fcleaned.$(RESET)"

re: fclean all

run: all
	./$(NAME)

git:
	git add .
	git commit -m "auto add & push"
	git push
	@echo "$(COLOR)git auto add & push performed.$(RESET)"

mgit:
	git add .
	@read -p "Enter the commit message: " msg;\
	git commit -m "$$msg"
	git push
	@echo "$(COLOR)git auto add & push with message performed.$(RESET)"

.SILENT: $(OBJ)