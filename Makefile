# webserv

# definitions
INC_DIR         =   inc
SRC_DIR         =   src
OBJ_DIR         =   obj
BIN_DIR         =   bin
NAME            =   webserv
CXX             =   c++
CXXFLAGS        =   -g3 -O0 -Wall -Wextra -Werror -std=c++98 -pedantic-errors -I$(INC_DIR) -MMD -MP

ifdef DEBUG
CXXFLAGS += -DDEBUG
endif

# sources and objects
SRCS            =   main.cpp \
                    Config/check_path.cpp \
                    Config/Config.cpp \
                    Handlers/AcceptHandler.cpp \
                    Handlers/ClientHandler.cpp \
                    HttpMessage.cpp \
                    HttpRequest/HttpRequestDecodeBody.cpp \
                    HttpRequest/HttpRequestDecodeChunked.cpp \
                    HttpRequest/HttpRequest.cpp \
                    HttpRequest/HttpRequestDecodeRequestLine.cpp \
                    HttpRequest/HttpRequestDecodeHeaders.cpp \
                    HttpResponse.cpp\
                    EventLoop.cpp\
                    Router/Router.cpp\
                    Router/getLocation.cpp\
                    Router/resolveBuildConfig.cpp\
                    Router/getPathAbsolute.cpp\
                    Router/checkMethod.cpp\
                    Router/getFileTypeFromPath.cpp\
                    Router/resolveFileSystem.cpp\
                    Router/resolveFileSystemDirectory.cpp\
                    Router/getCgiInfo.cpp\
                    Router/handleGet.cpp\
                    Router/directoryListing.cpp\
                    Router/handlePost.cpp\
                    Router/handleDelete.cpp \
                    Cgi/CgiContext.cpp \
                    Cgi/CgiLaunch.cpp \
                    Cgi/CgiReadHandler.cpp \
                    Cgi/CgiWriteHandler.cpp


OBJS            =   $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS            =   $(OBJS:.o=.d)

# compilation rule
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# default target
all: $(BIN_DIR)/$(NAME)

# must fclean before `make debug` if not debug
.PHONY:debug
debug:
	$(MAKE) DEBUG=1 all

# linking binary
$(BIN_DIR)/$(NAME): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	ln -sf $@ ./$(NAME)

# create symlink in sources/ to access binary
link:                   $(BIN_DIR)/$(NAME)
#                       ln -sf $(BIN_DIR)/$(NAME) ./$(NAME)

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

# include auto-generated header dependencies (silently ignored if missing)
-include $(DEPS)

# PHONY

.PHONY: all clean fclean re push pull fetch
