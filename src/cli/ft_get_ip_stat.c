
#include "cli.h"

t_ip			ft_get_ip(const char *command)
{
	int			i;
	t_ip		ip;
	char		*endptr;

	i = 0;
	ip.num = 0;
	endptr = NULL;
	while (command[i] && isspace(command[i]))
		++i;
	if (!command[i])
		return ((t_ip)0);
	ip = ft_parse_ip(command, &endptr);
	i = 0;
	while (endptr[i] && isspace(endptr[i]))
		++i;
	if (ip.num == 0 || !endptr[i] || strcmp(&(endptr[i]), "count") != 0)
		return ((t_ip)0);
	return (ip);
}

void			ft_get_ip_stat(int socket, t_ip ip)
{
	char	msg[MSG_SIZE];
	char	buffer[10];

	memset(msg, ' ', MSG_SIZE);

	/* command code - 2 */
	msg[0] = '2';

	msg[MSG_SIZE - 1] = '\n';
	memset(buffer, ' ', 10);
	snprintf(buffer, 10, "%d", ip.num);
	strncpy(&(msg[2]), buffer, 10);
	send(socket, msg, strlen(msg), 0);
}
