
CLI_NAME = cli
DMN_NAME = sniffer

INC = -I inc/
SRC_DIR = src src/*
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRC))

CC = gcc
CFLAGS = $(INC)
LDLIBS = -lpthread -lpcap

.PHONY: all clean fclean re
.NOTPARALLEL: all clean fclean re

all: $(OBJ) #$(CLI_NAME) $(DMN_NAME)

$(OBJ)/%.o: $(SRC)/%.c
    $(CC) $(CFLAGS) -c $< -o $@ $(LDLIBS)

$(CLI_NAME): $(CLI_OBJ)
	@$(CC) $(CLI_OBJ) -o $(CLI_NAME)

$(DMN_NAME): $(DMN_OBJ)
	@$(CC) $(DMN_OBJ) -o $(DMN_NAME)

obj/*/%.o: src/*/%.c
	$(CC) $(INC) -c $< -o $@ $(LDLIBS)

clean:
	@/bin/rm -f $(CLI_OBJ) $(DMN_OBJ)

fclean: clean
	@/bin/rm -f $(CLI_NAME) $(DMN_NAME)

re: fclean all
