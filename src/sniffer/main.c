
#include "sniff.h"

static void	ft_error(const char *msg)
{
	perror(msg);
	exit(1);
}

static void	ft_init_sarg(t_sniffer_arg *sarg)
{
	sarg->flags = 0;
	memset(sarg->iface, 0, IFACE_SIZE);
	strncpy(sarg->iface, DEFAULT_IFACE, strlen(DEFAULT_IFACE));
	sarg->ifaces = NULL;
}

static FILE	*ft_set_connection(void)
{
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


int			main(void)
{
	int				res;
    FILE			*fp;
    pthread_t		thread;
    t_sniffer_arg	sarg;

	ft_init_sarg(&sarg);
    pthread_create(&thread, NULL, ft_sniff, (void *)(&sarg));
    while (1)
	{
    	fp = ft_set_connection();
		res = ft_command_receiver(fp, &sarg);
		if (!res)
			break ;
	}
    pthread_join(thread, NULL);
	fclose(fp);
    return (0);
}
