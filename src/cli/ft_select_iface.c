
#include "cli.h"

void	ft_select_iface(
			int socket, const char *iface, char (*iface_ptr)[IFACE_SIZE])
{
	char	msg[MSG_SIZE];
	size_t	len;

	len = strchr(iface, '\n') - iface;
	printf("iface len: %lu\n", len);

	memset(*iface_ptr, 0, strlen(*iface_ptr));
	strncpy(*iface_ptr, iface, len);

	memset(msg, ' ', MSG_SIZE);

	/* command code - 3 */
	msg[0] = '3';
	msg[MSG_SIZE - 1] = '\n';

	strncpy(&(msg[2]), iface, len);
	send(socket, msg, strlen(msg), 0);
}
