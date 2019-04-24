
#include <signal.h>
#include <sys/mman.h>
#include "sniff.h"

t_sniffer_arg	*sarg;

static pcap_t	*ft_init_pcap(const char *iface, char *errbuf)
{
	pcap_t	*handle;

	handle = pcap_open_live(iface, BUFSIZ, 1, 1000, errbuf);
	if (handle == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", iface, errbuf);
		return (NULL);
	}
	return (handle);
}

static char		*ft_bind_to_iface(
					const char *iface,
					bpf_u_int32 *net,
					bpf_u_int32 *mask,
					char *errbuf)
{
	if (pcap_lookupnet(iface, net, mask, errbuf) == -1)
	{
		iface = DEFAULT_IFACE;
		if (pcap_lookupnet(iface, net, mask, errbuf) == -1)
		{
			iface = pcap_lookupdev(errbuf);
			if (pcap_lookupnet(iface, net, mask, errbuf) == -1)
			{
				fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
					iface, errbuf);
				*net = 0;
				*mask = 0;
			}
		}
	}
	sarg->pcap = ft_init_pcap(iface, errbuf);
	if (!sarg->pcap)
		return (NULL);
	else
		return ((char *)iface);
}

void			ft_interrupt(int sig)
{
	exit(EXIT_SUCCESS);
}

static void		ft_listen(t_list *iface_list)
{
	struct pcap_pkthdr	header;
	const u_char		*packet;
	t_sniff_ip*			ip;
	pid_t				pid;

	signal(SIGINT, ft_interrupt);
	pid = fork();
	while (1)
	{
		if (pid != 0)	/* parent */
		{
			if (sarg->flags & IS_TO_START)
			{
				sarg->flags |= IS_ACTIVE;
				sarg->flags &= ~IS_TO_START;
			}
			if (sarg->flags & IS_TO_STOP)
			{
				sarg->flags &= ~IS_ACTIVE;
				ft_write_log(LOGFILE, &(sarg->ifaces));
				sarg->flags &= ~IS_TO_STOP;
			}
			if (sarg->flags & IS_TO_EXIT)
			{
				kill(pid, SIGINT);
				return;
			}
			if (sarg->flags & IS_ACTIVE && sarg->stack_i)
			{
				while (sarg->flags & IS_WRITTING_STACK) ;

				sarg->flags |= IS_READING_STACK;
				for (--sarg->stack_i; sarg->stack_i >= 0; --sarg->stack_i)
				{
					if (sarg->ip_stack[sarg->stack_i].num)
					{
						ft_insert_ip(
							&(iface_list->bst), sarg->ip_stack[sarg->stack_i]);
						sarg->ip_stack[sarg->stack_i].num = 0;
					}
				}
				++sarg->stack_i;
				sarg->flags &= ~IS_READING_STACK;
				ft_write_log(LOGFILE, &(sarg->ifaces));
			}
			else
				bzero(sarg->ip_stack, IP_STACK_SIZE * sizeof(t_ip));
			usleep(100000);
		}
		else			/* child */
		{
			packet = pcap_next(sarg->pcap, &header);
			ip = (t_sniff_ip *)(packet + SIZE_ETHERNET);
			if (sarg->flags & IS_ACTIVE)
			{
				while (sarg->flags & IS_READING_STACK) ;

				sarg->flags |= IS_WRITTING_STACK;
				sarg->ip_stack[sarg->stack_i++].num = ip->ip_src.s_addr;
				if (sarg->stack_i == IP_STACK_SIZE)
					--sarg->stack_i;
				sarg->flags &= ~IS_WRITTING_STACK;
			}
		}
	}
}

void			*ft_sniff(void *arg)
{
	char				iface[IFACE_SIZE];
	char				*binded_iface;
	char				errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32			mask;
	bpf_u_int32			net;
	FILE				*logfile;
	t_list				*iface_list;

	sarg = (t_sniffer_arg *)arg;

	/* initializing ip tree from log file */

	logfile = fopen(LOGFILE, "r");
	if (logfile > 0)
	{
		ft_init_from_file(&(sarg->ifaces), logfile);
		fclose(logfile);
	}

	/* initializing pcap data */

	bzero(iface, IFACE_SIZE);
	binded_iface = ft_bind_to_iface(sarg->iface, &net, &mask, errbuf);
	if (!binded_iface)
		return (NULL);
	strcpy(iface, binded_iface);
	iface_list = ft_search_iface(&(sarg->ifaces), iface);
	if (!sarg->pcap)
		return (NULL);

	/* listening */

	ft_listen(iface_list);

	return (NULL);
}
