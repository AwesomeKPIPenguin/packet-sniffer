
#include "cli.h"

int		main(void)
{
    int				sock;
    unsigned int	len;
    t_sockaddr_un	addr;

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("cli: socket");
        exit(1);
    }
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, ADDRESS);
    len = sizeof(addr.sun_family) + strlen(addr.sun_path);
    if (connect(sock, (struct sockaddr *)&addr, len) < 0)
	{
    	system("./daemon &");
    	usleep(100000);

    	if (connect(sock, (struct sockaddr *)&addr, len) < 0)
		{
    		system("killall daemon");
        	perror("cli: connect");
        	exit(1);
		}
    }

    ft_listen_for_commands(sock);

    close(sock);
	return (0);
}
