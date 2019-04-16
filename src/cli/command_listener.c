
#include "cli.h"

void	ft_usage()
{
	printf("Usage: \n");
}

void	ft_listen_for_commands(int socket)
{
	char		command[BUFF_SIZE];
	char		iface[IFACE_SIZE];
	t_ip		ip;

	strcpy(iface, DEFAULT_IFACE);
	ip.num = 0;
	while (1)
	{
		printf("Selected iface: %s\n", iface);
		printf(" > ");
		memset(command, 0, BUFF_SIZE);
		fgets(command, BUFF_SIZE, stdin);
		printf("Received command: %s", command);

		if (strncmp(command, "start", 5) == 0)
			ft_start(socket);
		else if (strncmp(command, "stop", 4) == 0)
			ft_stop(socket);
		else if (strncmp(command, "show ", 5) == 0 && command[5])
		{
			ip = ft_get_ip(&(command[5]));
			if (ip.num == 0)
				ft_usage();
			else
				ft_get_ip_stat(socket, ip);
		}
		else if (strncmp(command, "select iface ", 13) == 0 && command[13])
			ft_select_iface(socket, &(command[13]), &iface);
		else if (strncmp(command, "stat ", 5) == 0 && command[5])
			ft_iface_stat(socket, &(command[5]));
		else if (strncmp(command, "exit", 4) == 0)
			break ;
		else
			ft_usage();
	}
}
