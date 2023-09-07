NAME	=	webserv

CC		=	c++
RM		=	rm -rf
FLAGS	=	-Wall -Wextra -Werror -pedantic -std=c++98 #-fsanitize=address

COLOR	=	\033[30m
RESET	=	\033[0m

SRCFILE	=	main.cpp\
			Binding.cpp\
			Request.cpp\
			Client.cpp\
			Config.cpp\
			ConfigFile.cpp\
			ErrorCode.exception.cpp\
			NetworkFailure.exception.cpp\
			Response.cpp\
			Server.cpp\
			webserv.cpp
SRC		=	$(addprefix src/, $(SRCFILE))
OBJ		= 	$(addprefix obj/, $(SRCFILE:%.cpp=%.o))

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(FLAGS) $(OBJ) -o $(NAME) 
	@echo "$(COLOR)$(NAME) compiled.$(RESET)"

obj/%.o: src/%.cpp
	@mkdir -p obj/
	$(CC) $(FLAGS) -c $< -o $@

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