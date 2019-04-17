
CLI_NAME = cli
DMN_NAME = sniffer

INC = -I inc/

CLI_SRC = src/utils.c $(wildcard src/cli/*.c)
DMN_SRC = src/utils.c $(wildcard src/sniffer/*.c)

CC = gcc
CFLAGS = $(INC)
LDLIBS = -lpthread -lpcap

.PHONY: all clean fclean re
.NOTPARALLEL: all clean fclean re

all:
	@$(CC) $(CFLAGS) $(CLI_SRC) -o $(CLI_NAME)
	@$(CC) $(CFLAGS) $(DMN_SRC) -o $(DMN_NAME) $(LDLIBS)

#$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
#	$(CC) $(CFLAGS) -c $< -o $@ $(LDLIBS)

#obj/*/%.o: src/*/%.c
#	$(CC) $(INC) -c $< -o $@ $(LDLIBS)

clean:
	@$(RM) $(CLI_OBJ) $(DMN_OBJ)

fclean: clean
	@$(RM) $(CLI_NAME) $(DMN_NAME)

re: fclean all
