
#include "cli.h"

void	ft_start(int socket)
{
	/* command code - 0 */
	send(socket, "0\n", 2, 0);
}

void	ft_stop(int socket)
{
	/* command code - 1 */
	send(socket, "1\n", 2, 0);
}

t_ip	ft_get_ip(const char *command)
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
		return ((t_ip)0u);
	ip = ft_parse_ip(command, &endptr);
	i = 0;
	while (endptr[i] && isspace(endptr[i]))
		++i;
	if (ip.num == 0 || !endptr[i]
		|| strncmp(&(endptr[i]), "count", 5) != 0 || endptr[i + 5] != '\n')
		return ((t_ip)0u);
	return (ip);
}

void	ft_get_ip_stat(int socket, t_ip ip)
{
	char	msg[MSG_SIZE];

	bzero(msg, MSG_SIZE);

	/* command code - 2 */
	sprintf(msg, "2 %u\n", ip.num);

	send(socket, msg, strlen(msg), 0);
}

void	ft_select_iface(
			int socket, const char *iface, char (*iface_ptr)[IFACE_SIZE])
{
	char	msg[MSG_SIZE];
	size_t	len;

	len = strchr(iface, '\n') - iface;
	bzero(*iface_ptr, strlen(*iface_ptr));
	strncpy(*iface_ptr, iface, len);
	bzero(msg, MSG_SIZE);

	/* command code - 3 */
	sprintf(msg, "3 %s\n", *iface_ptr);

	printf("Sent: %s\n", msg);

	send(socket, msg, strlen(msg), 0);
}

void	ft_iface_stat(int socket, const char *iface)
{
	char	msg[MSG_SIZE];

	memset(msg, ' ', MSG_SIZE);

	/* command code - 4 */
	msg[0] = '4';

	msg[MSG_SIZE - 1] = '\n';
	strncpy(&(msg[2]), iface, strlen(iface));
	send(socket, msg, strlen(msg), 0);
}

void	ft_kill(int socket)
{
	/* command code - 5 */
	send(socket, "5\n", 2, 0);
}