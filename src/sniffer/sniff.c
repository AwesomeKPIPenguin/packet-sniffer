
#include <signal.h>
#include <sys/mman.h>
#include "sniff.h"

t_sniffer_arg		*sarg;

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
	return ((char *)iface);
}

static pcap_t	*ft_init_pcap(const char *iface, bpf_u_int32 net, char *errbuf)
{
	pcap_t				*handle;
	struct bpf_program	fp;
	char				filter_exp[] = "";

	handle = pcap_open_live(iface, BUFSIZ, 1, 1000, errbuf);
	if (handle == NULL)
	{
		fprintf(stderr, "Couldn't open device %s: %s\n",
			iface, errbuf);
		return (NULL);
	}
	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1)
	{
		fprintf(stderr, "Couldn't parse filter %s: %s\n",
			filter_exp, pcap_geterr(handle));
		return (NULL);
	}
	if (pcap_setfilter(handle, &fp) == -1)
	{
		fprintf(stderr, "Couldn't install filter %s: %s\n",
			filter_exp, pcap_geterr(handle));
		return (NULL);
	}
	return (handle);
}

void			ft_interrupt(int sig)
{
	exit(EXIT_SUCCESS);
}

static void		ft_listen(char *iface, bpf_u_int32 *mask, bpf_u_int32 *net, char *errbuf, t_list *iface_list)
{
	struct pcap_pkthdr	header;
	const u_char		*packet;
	t_sniff_ip*			ip;
	pid_t				pid;

	signal(SIGINT, ft_interrupt);
	pid = fork();
	while (1)
//	for (int i = 0; i < 10; ++i)
	{
		if (pid != 0)	/* parent */
		{
//			printf("Parent\n");
			if (sarg->flags & IS_TO_START)
			{
				sarg->flags |= IS_ACTIVE;
				printf("Starting, is active in parent = %u\n", sarg->flags & IS_ACTIVE);
				sarg->flags &= ~IS_TO_START;
			}
			if (sarg->flags & IS_TO_STOP)
			{
				printf("Stopping\n");
				sarg->flags &= ~IS_ACTIVE;
				printf("Flag changed\n");
				ft_write_log(LOGFILE, &(sarg->ifaces));
				printf("Logged\n");
				sarg->flags &= ~IS_TO_STOP;
				printf("Flag changed\n");
			}
			if (strcmp(sarg->iface, iface) != 0)
				iface = ft_bind_to_iface(sarg->iface, net, mask, errbuf);
			if (sarg->flags & IS_TO_EXIT)
			{
				printf("Killing child\n");
//				ft_write_log(LOGFILE, &(sarg->ifaces));
				kill(pid, SIGINT);
				return;
			}
			if (sarg->flags & IS_ACTIVE && sarg->stack_i)
			{
				printf("stack_i: %d\n", sarg->stack_i);

				for (--sarg->stack_i; sarg->stack_i >= 0; --sarg->stack_i)
				{
					printf("stack[%d]: %hhu.%hhu.%hhu.%hhu\n",
						sarg->stack_i,
						sarg->ip_stack[sarg->stack_i].bytes[0],
						sarg->ip_stack[sarg->stack_i].bytes[1],
						sarg->ip_stack[sarg->stack_i].bytes[2],
						sarg->ip_stack[sarg->stack_i].bytes[3]);

					if (sarg->ip_stack[sarg->stack_i].num)
					{
						ft_insert_ip(
							&(iface_list->bst), sarg->ip_stack[sarg->stack_i]);
						sarg->ip_stack[sarg->stack_i].num = 0;
					}
				}
				++sarg->stack_i;
				ft_write_log(LOGFILE, &(sarg->ifaces));
			}
			else
				bzero(sarg->ip_stack, IP_STACK_SIZE * sizeof(t_ip));
			usleep(100000);
		}
		else			/* child */
		{
//			printf("Receiving packet\n");
			packet = pcap_next(sarg->pcap, &header);
//			printf("Passed packet recv\n");
//			printf("Is active in child = %u\n", sarg->flags & IS_ACTIVE);
			ip = (t_sniff_ip *)(packet + SIZE_ETHERNET);
			printf("RECV: %s\n", inet_ntoa(ip->ip_src));
			if (sarg->flags & IS_ACTIVE)
			{
				sarg->ip_stack[sarg->stack_i++].num = ip->ip_src.s_addr;
				if (sarg->stack_i == IP_STACK_SIZE)
					--sarg->stack_i;
			}
//			ft_insert_ip(&(iface_list->bst), source_ip);
//			ft_write_log(LOGFILE, &(sarg->ifaces));
		}
	}
}

void			*ft_sniff(void *arg)
{
	char				*iface;
	char				errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32			mask;
	bpf_u_int32			net;
	FILE				*logfile;
	t_list				*iface_list;

	sarg = (t_sniffer_arg *)arg;

//	FILE *response_fp = fopen("response.txt", "a+");
//	sarg->response_fp = response_fp;
//	sarg->is_to_send_iface_stat = 1;
//	strcpy(sarg->iface_for_stat, "jopa");

	/* initializing ip_for_stat tree from log file */

	logfile = fopen(LOGFILE, "r");
	if (logfile > 0)
	{
		ft_init_from_file(&(sarg->ifaces), logfile);
		fclose(logfile);
	}

	/* initializing pcap data */

	iface = ft_bind_to_iface(sarg->iface, &net, &mask, errbuf);
	iface_list = ft_search_iface(&(sarg->ifaces), iface);
	sarg->pcap = ft_init_pcap(iface, net, errbuf);
	if (!sarg->pcap)
		return (NULL);

	/* listening */

	ft_listen(iface, &mask, &net, errbuf, iface_list);

	return (NULL);
}
