
#ifndef SNIFF_H

# define SNIFF_H

# include "list.h"
# include "utils.h"
# include <pcap.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <pthread.h>
# include <netinet/ip_icmp.h>
# include <netinet/udp.h>
# include <netinet/tcp.h>
# include <netinet/ip.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/un.h>
# include <sys/ioctl.h>
# include <net/if.h>
# include <linux/types.h>
# include <linux/if_ether.h>
# include <linux/if_packet.h>
# include <arpa/inet.h>

# define DEFAULT_IFACE			"eth0"
# define MSG_SIZE				16
# define ADDRESS				"sniff"
# define LOGFILE				"log.txt"
# define SIZE_ETHERNET			14
# define IP_STACK_SIZE			1024

# define IS_TO_START			0x8000u
# define IS_TO_STOP				0x4000u
# define IS_ACTIVE				0x2000u
# define IS_TO_LOG				0x1000u
# define IS_TO_SEND_IP_STAT		0x0800u
# define IS_TO_SEND_IFACE_STAT	0x0400u
# define IS_TO_EXIT				0x0200u
# define IS_READING_STACK		0x0100u
# define IS_WRITTING_STACK		0x0080u

typedef struct sockaddr_un	t_sockaddr_un;

typedef struct				s_sniff_ip {
	u_char					ip_vhl;				/* ver << 4 | hdr len >> 2 */
	u_char					ip_tos;				/* type of service */
	u_short					ip_len;				/* total length */
	u_short					ip_id;				/* identification */
	u_short					ip_off;				/* fragment offset field */
# define					IP_RF		0x8000	/* reserved fragment flag */
# define					IP_DF		0x4000	/* dont fragment flag */
# define					IP_MF		0x2000	/* more fragments flag */
# define					IP_OFFMASK	0x1fff	/* mask for fragmenting bits */
	u_char					ip_ttl;				/* time to live */
	u_char					ip_p;				/* protocol */
	u_short					ip_sum;				/* checksum */
	struct in_addr			ip_src;				/* src ip_for_stat address */
	struct in_addr			ip_dst;				/* dst ip_for_stat address */
}							t_sniff_ip;

typedef struct				s_sniffer_arg
{
	uint16_t				flags;
	t_ip					ip_stack[IP_STACK_SIZE];
	int						stack_i;
	t_ip					ip_for_stat;
	char					iface[IFACE_SIZE];
	char					iface_for_stat[IFACE_SIZE];
	pcap_t					*pcap;
	t_list					*ifaces;
	int						response_fd;
}							t_sniffer_arg;

int							ft_command_receiver(FILE *fp, t_sniffer_arg *sarg);
void						*ft_sniff(void *arg);

#endif /* SNIFF_H */
