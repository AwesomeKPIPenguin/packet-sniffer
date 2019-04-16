
daemon:
	gcc -I inc src/sniffer/*.c src/cli/utils.c -o daemon -lpthread -lpcap
