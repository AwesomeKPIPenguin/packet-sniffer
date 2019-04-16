
#include "cli.h"

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
