
#include "cli.h"

void	ft_usage()
{
	printf("Usage: \n");
}

void	ft_listen_for_commands(int socket)
{
	int			is_running;
	t_ip		ip;
	char		command[BUFF_SIZE];
	char		iface[IFACE_SIZE];
	size_t		cmd_len;

	is_running = 1;
	strcpy(iface, DEFAULT_IFACE);
	ip.num = 0;
	while (1)
	{
//		printf("Selected iface: %s\n", iface);
		printf(" > ");
		memset(command, 0, BUFF_SIZE);
		fgets(command, BUFF_SIZE, stdin);
//		printf("Received command: %s", command);
		cmd_len = strchr(command, '\n') - command;

		if (strncmp(command, "start", cmd_len) == 0)
			ft_start(socket);
		else if (strncmp(command, "stop", cmd_len) == 0)
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
		else if (strncmp(command, "run", cmd_len) == 0)
		{
			if (!is_running)
				system("./daemon &");
			is_running = 1;
		}
		else if (strncmp(command, "kill", cmd_len) == 0)
		{
			if (is_running)
			{
				is_running = 0;
				ft_kill(socket);
			}
		}
		else if (strncmp(command, "exit", cmd_len) == 0)
			break ;
		else
			ft_usage();
	}
}
