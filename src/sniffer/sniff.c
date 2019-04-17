
#include "sniff.h"

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

void			*ft_sniff(void *arg)
{
	pcap_t				*handle;
	char				*iface;
	char				errbuf[PCAP_ERRBUF_SIZE];
	bpf_u_int32			mask;
	bpf_u_int32			net;
	struct pcap_pkthdr	header;
	const u_char		*packet;
	t_sniff_ip*			ip;
	t_sniffer_arg		*sarg;
	FILE				*logfile;
	t_list				*iface_list;
	t_ip				source_ip;

	sarg = (t_sniffer_arg *)arg;

//	FILE *response_fp = fopen("response.txt", "a+");
//	sarg->response_fp = response_fp;
//	sarg->is_to_send_iface_stat = 1;
//	strcpy(sarg->iface_for_stat, "jopa");

	/* initializing ip tree from log file */

	logfile = fopen(LOGFILE, "a+");
	if (logfile < 0)
	{
		fprintf(stderr, "Log file opening/creation failed\n");
		return (NULL);
	}
	ft_init_from_file(&(sarg->ifaces), logfile);
	fclose(logfile);

	/* initializing pcap data */

	iface = ft_bind_to_iface(sarg->iface, &net, &mask, errbuf);
	iface_list = ft_search_iface(&(sarg->ifaces), iface);
	handle = ft_init_pcap(iface, net, errbuf);
	if (!handle)
		return (NULL);

	/* listening */

    while (1)
//	for (int i = 0; i < 10; ++i)
    {
		if (strcmp(sarg->iface, iface) != 0)
			iface = ft_bind_to_iface(sarg->iface, &net, &mask, errbuf);
		if (sarg->flags & IS_TO_SEND_IP_STAT)
		{
			ft_send_ip_stat(&(sarg->ifaces), sarg->ip, sarg->response_fp);
			sarg->flags &= ~IS_TO_SEND_IP_STAT;
		}
		if (sarg->flags & IS_TO_SEND_IFACE_STAT)
		{
			ft_send_iface_stat(
				&(sarg->ifaces), sarg->iface_for_stat, sarg->response_fp);
			sarg->flags &= ~IS_TO_SEND_IFACE_STAT;
		}
		if (sarg->flags & IS_TO_EXIT)
		{
			ft_write_log(LOGFILE, &(sarg->ifaces));
			return (NULL);
		}
    	if (!(sarg->flags & IS_ACTIVE))
		{
            printf("Sleeping...\n");
    		if (sarg->flags & IS_TO_LOG)
			{
				ft_write_log(LOGFILE, &(sarg->ifaces));
				sarg->flags &= ~IS_TO_LOG;
			}
    		usleep(500000); /* 0.5s */
			continue ;
		}

        printf("Active\n");

		packet = pcap_next(handle, &header);
		ip = (t_sniff_ip *)(packet + SIZE_ETHERNET);
//		printf("source ip: %s\n", inet_ntoa(ip->ip_src));
		source_ip.num = ip->ip_src.s_addr;
        ft_insert_ip(&(iface_list->bst), source_ip);
//		ft_write_log(LOGFILE, &(sarg->ifaces));
    }
	return (NULL);
}
