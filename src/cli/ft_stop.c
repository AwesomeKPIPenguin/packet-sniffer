
#include "cli.h"

void	ft_stop(int socket)
{
	/* command code - 1 */
	send(socket, "1\n", 2, 0);
}
