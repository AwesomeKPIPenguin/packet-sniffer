
#include "sniff.h"

static int	ft_handle_command(t_sniffer_arg *sarg, const char cmd[MSG_SIZE])
{
	if (cmd[0] == '0')		/* start */
	{
		if (!(sarg->flags & IS_ACTIVE))
			sarg->flags |= IS_TO_START;
	}
	else if (cmd[0] == '1')	/* stop */
	{
		if (sarg->flags & IS_ACTIVE)
		{
			sarg->flags |= IS_TO_LOG;
			sarg->flags |= IS_TO_STOP;
		}
	}
	else if (cmd[0] == '2')	/* show [ip_for_stat] count */
	{
		sarg->ip_for_stat.num = strtoul(&(cmd[2]), NULL, 10);
		sarg->flags |= IS_TO_SEND_IP_STAT;
		ft_send_ip_stat(&(sarg->ifaces), sarg->ip_for_stat, sarg->response_fd);
		sarg->flags &= ~IS_TO_SEND_IP_STAT;
	}
	else if (cmd[0] == '4')	/* stat [iface] */
	{
		bzero(sarg->iface_for_stat, IFACE_SIZE);
		strncpy(sarg->iface_for_stat, &(cmd[2]), strchr(cmd, '\n') - &(cmd[2]));
		sarg->flags |= IS_TO_SEND_IFACE_STAT;
		ft_send_iface_stat(
			&(sarg->ifaces), sarg->iface_for_stat, sarg->response_fd);
		sarg->flags &= ~IS_TO_SEND_IFACE_STAT;
	}
	else if (cmd[0] == '3' || cmd[0] == '5') /* select iface [iface]; kill */
	{
		sarg->flags |= IS_TO_EXIT;
		return (0);
	}
	return (1);
}

int			ft_command_receiver(FILE *fp, t_sniffer_arg *sarg)
{
	int		c;
	char 	buffer[MSG_SIZE];

	while (1)
	{
		bzero(buffer, MSG_SIZE);
		c = 0;
		for (int i = 0; c != '\n'; ++i)
		{
			if ((c = fgetc(fp)) < 0)
				return (1);
			buffer[i] = (char)c;
		}
		if (!ft_handle_command(sarg, buffer))
			return (0);
	}
}
