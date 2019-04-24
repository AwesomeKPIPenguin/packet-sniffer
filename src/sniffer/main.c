
#include <sys/mman.h>
#include "sniff.h"

static void				ft_error(const char *msg)
{
	perror(msg);
	exit(1);
}

static void				ft_free_tree(t_node *node)
{
	if (node->left)
		ft_free_tree(node->left);
	if (node->right)
		ft_free_tree(node->right);
	munmap(node, sizeof(t_node));
}

static void				ft_free_list(t_list *node)
{
	if (node->next)
		ft_free_list(node->next);
	munmap(node, sizeof(t_list));
}

static void				ft_free_sarg(t_sniffer_arg *sarg)
{
	ft_free_list(sarg->ifaces);
	munmap(sarg, sizeof(t_sniffer_arg));
}

static t_sniffer_arg	*ft_init_sarg(char *iface)
{
	t_sniffer_arg	*sarg;

	sarg = (t_sniffer_arg *)mmap(
		NULL,
		sizeof(t_sniffer_arg),
		PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_ANONYMOUS,
		0,
		0);
	sarg->flags = 0;
	sarg->stack_i = 0;
	sarg->pcap = NULL;
	sarg->ifaces = NULL;
	sarg->response_fd = 0;
	bzero(sarg->iface, IFACE_SIZE);
	bzero(sarg->ip_stack, IP_STACK_SIZE * sizeof(t_ip));
	strncpy(sarg->iface, iface, strlen(iface));
	return (sarg);
}

static FILE				*ft_set_connection(int *cli_socket_fd)
{
    int				socket_fd;
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
    *cli_socket_fd = accept(
    	socket_fd, (struct sockaddr *)&cli_sockaddr, &fromlen);
    if (*cli_socket_fd < 0)
    	ft_error("daemon: accept");
    fp = fdopen(*cli_socket_fd, "r");
	if (!fp)
    	ft_error("daemon: fdopen");
	return (fp);
}


int						main(int argc, char **argv)
{
	int				res;
	int				socket;
    FILE			*fp;
    pthread_t		thread;
    t_sniffer_arg	*sarg;

	if (argc > 2)
	{
		printf("Error: Too many arguments\n");
		return (1);
	}
	else if (argc == 2)
		sarg = ft_init_sarg(argv[1]);
	else
		sarg = ft_init_sarg(DEFAULT_IFACE);

	printf("Iface: %s\n", sarg->iface);

	pthread_create(&thread, NULL, ft_sniff, (void *)(sarg));
	while (1)
	{
		fp = ft_set_connection(&socket);
		sarg->response_fd = socket;
		res = ft_command_receiver(fp, sarg);
		if (!res)
			break ;
	}
    pthread_join(thread, NULL);
    ft_free_sarg(sarg);
	fclose(fp);
    return (0);
}
