
#include "cli.h"

static void	ft_usage()
{
	printf("Usage: start\n");
	printf("       stop\n");
	printf("       show [ip] count\n");
	printf("       select iface [iface]\n");
	printf("       stat [iface]\n");
	printf("       run\n");
	printf("       kill\n");
	printf("       --help\n");
}

static void	ft_help()
{
	printf("Usage: start                - sniffed packets starts counting (default iface)\n");
	printf("       stop                 - sniffed packets stops counting\n");
	printf("       show [ip] count      - shows the number of packets, received from [ip]\n");
	printf("       select iface [iface] - select interface for sniffing\n");
	printf("       stat [iface]         - show statistic for [iface]\n");
	printf("       run                  - starts the sniffer process\n");
	printf("       kill                 - kills the sniffer process\n");
	printf("       --help               - this message is shown\n");
}

void		ft_listen_for_commands(
				int socket, t_sockaddr_un *addr, unsigned int len)
{
	int			is_running;
	t_ip		ip;
	char		command[BUFF_SIZE];
	char		iface[IFACE_SIZE];
	size_t		cmd_len;
	pid_t		pid;
	FILE		*fp;
	char		line[128];
	int			c;

	is_running = 1;
	strcpy(iface, DEFAULT_IFACE);
	ip.num = 0;
	pid = fork();
	if (pid > 0)	/* parent */
	{
		while (1)
		{
			printf(" > ");
			bzero(command, BUFF_SIZE);
			fgets(command, BUFF_SIZE, stdin);
			cmd_len = strchr(command, '\n') - command;

			if (cmd_len == 5
				&& strncmp(command, "start", cmd_len) == 0)
				ft_start(socket);
			else if (cmd_len == 4
					&& strncmp(command, "stop", cmd_len) == 0)
				ft_stop(socket);
			else if (cmd_len >= 18 && cmd_len <= 26
					&& strncmp(command, "show ", 5) == 0)
			{
				ip = ft_get_ip(&(command[5]));
				if (ip.num == 0)
					ft_usage();
				else
					ft_get_ip_stat(socket, ip);
			}
			else if (cmd_len >= 14
					&& strncmp(command, "select iface ", 13) == 0)
			{
				ft_select_iface(socket, &(command[13]), &iface);
				connect(socket, (struct sockaddr *)&addr, len);
			}
			else if (cmd_len >= 6
					&& strncmp(command, "stat ", 5) == 0)
				ft_iface_stat(socket, &(command[5]));
			else if (strncmp(command, "run", cmd_len) == 0)
			{
				if (!is_running)
				{
					system("./cli");
					exit(EXIT_SUCCESS);
				}
			}
			else if (cmd_len == 4
					&& strncmp(command, "kill", cmd_len) == 0)
			{
				if (is_running)
				{
					is_running = 0;
					ft_kill(socket);
				}
			}
			else if (cmd_len == 4
					&& strncmp(command, "exit", cmd_len) == 0)
				break;
			else if (cmd_len == 6
					 && strncmp(command, "--help", cmd_len) == 0)
				ft_help();
			else
				ft_usage();
		}
	}
	else			/* child */
	{
		fp = fdopen(socket, "r");
		while (1)
		{
			bzero(line, 128);
			c = 0;
			for (int i = 0; c != '\n'; ++i)
			{
				if ((c = fgetc(fp)) < 0)
					return ;
				line[i] = (char)c;
			}
			printf("%s", line);
		}
	}
}
