# webserv

# definitions
INC_DIR			=	inc
SRC_DIR			=	src
OBJ_DIR			=	obj
BIN_DIR			=	bin
NAME			=	webserv
CXX				=	c++
CXXFLAGS		=	-Wall -Wextra -Werror -std=c++98 -pedantic-errors -I$(INC_DIR)

# sources and objects
SRCS			=	mainHandler.cpp \
					HttpMessage.cpp \
					HttpRequest/HttpRequestDecodeBody.cpp \
					HttpRequest/HttpRequestDecodeChunked.cpp \
					HttpRequest/HttpRequest.cpp \
					HttpRequest/HttpRequestDecodeRequestLine.cpp \
					HttpRequest/HttpRequestDecodeHeaders.cpp \
					Handlers/ClientHandler.cpp


OBJS			=	$(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# compilation rule
$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# default target
all: $(BIN_DIR)/$(NAME)

# linking binary
$(BIN_DIR)/$(NAME): $(OBJS)
						mkdir -p $(BIN_DIR)
						$(CXX) $(CXXFLAGS) $(OBJS) -o $@
						ln -sf $@ ./$(NAME)

# create symlink in sources/ to access binary
link:					$(BIN_DIR)/$(NAME)
#						ln -sf $(BIN_DIR)/$(NAME) ./$(NAME)

# cleaning
#removes content of /bin and /obj directories
clean:
						rm -rf $(OBJ_DIR)/*
						rm -rf $(BIN_DIR)/*
						
#removes content and directories /bin and /obj
fclean: clean
						rm -rf $(OBJ_DIR)
						rm -rf $(BIN_DIR)
						rm -rf $(NAME)

re: fclean all

# git

# git log
log:
	git log --oneline --graph --decorate --all

# git commit + push
push: fclean
	@DATE=$$(date "+%Y-%m-%d %Hh%Mm%S -"); \
	USER=$$(whoami)" -"; \
	printf "enter commit message > "; \
	read MSG; \
	MSG=$${MSG:-update}; \
	git add -A; \
	git commit -m "$$USER $$DATE $$MSG" || echo "nothing to commit"; \
	git push

# git pull
pull:
	git pull

# git fetch + pull
fetch:
	git fetch
	make -s pull


# PHONY

.PHONY: all clean fclean re push pull fetch
