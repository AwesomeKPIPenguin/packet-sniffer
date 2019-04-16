
#include "sniff.h"

void	ft_error(const char *msg)
{
	perror(msg);
	exit(1);
}

FILE	*ft_set_connection(void)
{
	printf("in set connection\n");

    int				socket_fd;
    int				cli_socket_fd;
    unsigned int	fromlen;
    unsigned int	len;
    FILE			*fp;
    t_sockaddr_un	sockaddr;
    t_sockaddr_un	cli_sockaddr;

    if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		ft_error("daemon: socket");
    sockaddr.sun_family = AF_UNIX;
    strcpy(sockaddr.sun_path, ADDRESS);
    unlink(ADDRESS);
    len = sizeof(sockaddr.sun_family) + strlen(sockaddr.sun_path);
    fromlen = len;
    if (bind(socket_fd, (struct sockaddr *)&sockaddr, len) < 0)
		ft_error("daemon: bind");
    if (listen(socket_fd, 5) < 0)
    	ft_error("daemon: listen");
    cli_socket_fd = accept(
    	socket_fd, (struct sockaddr *)&cli_sockaddr, &fromlen);
    if (cli_socket_fd < 0)
    	ft_error("daemon: accept");
    fp = fdopen(cli_socket_fd, "r");
	if (!fp)
    	ft_error("daemon: fdopen");
//	setvbuf(fp, NULL, _IONBF, 0);

	return (fp);
}

void	ft_command_receiver(FILE *fp)
{
	int		c;
    char	buffer[MSG_SIZE];

	while (1)
	{
		memset(buffer, 0, MSG_SIZE);
		c = 0;
		for (int i = 0; c != '\n'; ++i)
		{
//			printf("in for BEFORE fgetc\n");
			if ((c = fgetc(fp)) < 0)
				return ;
//			printf("in for AFTER fgetc\n");
			buffer[i] = (char)c;
		}
		if (buffer[0])
			printf("Received command: %s\n", buffer);

		//ft_process_command(buffer);
	}
}

int		main(void)
{
    FILE			*fp;
    pthread_t		thread;
    t_sniffer_arg	sarg;

    sarg.is_active = 1;
    sarg.is_to_log = 0;
    memset(sarg.iface, 0, IFACE_SIZE);
    strncpy(sarg.iface, DEFAULT_IFACE, strlen(DEFAULT_IFACE));
    sarg.ifaces = NULL;
    pthread_create(&thread, NULL, ft_sniff, (void *)(&sarg));

    while (1)
	{
    	fp = ft_set_connection();
		ft_command_receiver(fp);
	}
//
//	close(sock);
//
//	exit(0);
//	return (0);
}
