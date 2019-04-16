
#include "cli.h"

void	ft_start(int socket)
{
	/* command code - 0 */
	send(socket, "0\n", 2, 0);
}
