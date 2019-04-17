
#include "sniff.h"

static int	ft_handle_command(t_sniffer_arg *sarg, const char cmd[MSG_SIZE])
{
	if (cmd[0] == '0')		/* start */
	{
		sarg->flags |= IS_ACTIVE;
	}
	else if (cmd[0] == '1')	/* stop */
	{
		sarg->flags |= IS_TO_LOG;
		sarg->flags &= ~IS_ACTIVE;
	}
	else if (cmd[0] == '2')	/* show [ip] count */
	{
		sarg->ip.num = strtoul(&(cmd[2]), NULL, 10);
		sarg->flags |= IS_TO_SEND_IP_STAT;
	}
	else if (cmd[0] == '3')	/* select [iface] */
	{
		strncpy(sarg->iface, &(cmd[2]), IFACE_SIZE);
	}
	else if (cmd[0] == '4')	/* stat [iface] */
	{
		strncpy(sarg->iface_for_stat, &(cmd[2]), IFACE_SIZE);
		sarg->flags |= IS_TO_SEND_IFACE_STAT;
	}
	else if (cmd[0] == '5')	/* kill */
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
		memset(buffer, 0, MSG_SIZE);
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
