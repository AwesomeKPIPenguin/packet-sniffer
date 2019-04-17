
#include "list.h"
#include "utils.h"
#include "bst.h"

static t_list	*ft_list_nodenew(const char *iface)
{
	t_list	*node;

	node = (t_list *)malloc(sizeof(t_list));
	bzero(node->iface, IFACE_SIZE);
	strncpy(node->iface, iface, strlen(iface));
	node->bst = NULL;
	node->next = NULL;

	return (node);
}

t_node			*ft_parse_line(const char *s, char (*iface)[IFACE_SIZE])
{
	int		i;
	char	*ptr;
	t_node	*node;

	i = -1;
	ptr = NULL;
	while (s[++i] != ' ' && s[i] != ':')
		(*iface)[i] = s[i];
	while (s[++i] == ' ' || s[i] == ':') ;
	node = ft_nodenew(
		ft_parse_ip(&(s[i]), &ptr));
	while (*(++ptr) == ' ' || *ptr == ':') ;
	node->count = (int)strtol(ptr, NULL, 10);

	return (node);
}

t_list			*ft_search_iface(t_list **head, const char *iface)
{
	t_list	*prev;
	t_list	*node;

	node = *head;
	prev = node;
	while (node)
	{
		prev = node;
		if (strcmp(node->iface, iface) == 0)
			return (node);
		node = node->next;
	}
	if (prev)
	{
		prev->next = ft_list_nodenew(iface);
		return (prev->next);
	}
	else
	{
		*head = ft_list_nodenew(iface);
		return (*head);
	}
}

void			ft_init_from_file(t_list **head, FILE *fp)
{
	char	s[LOG_LINE_SIZE];
	char	iface[IFACE_SIZE];
	t_list	*lnode;
	t_node	*node;

	while (fgets(s, LOG_LINE_SIZE + 1, fp))
	{
		bzero(iface, IFACE_SIZE);
		node = ft_parse_line(s, &iface);
		lnode = ft_search_iface(head, iface);
		ft_insert_node(&(lnode->bst), node);
		bzero(s, LOG_LINE_SIZE);
	}
}

void			ft_write_log(const char *filename, t_list **head)
{
	FILE	*fp;
	t_list	*lnode;

	fp = fopen(filename, "w+");

//	fprintf(fp, " --- NEW WRITE LOG ---\n");

	lnode = *head;
	while (lnode)
	{
		ft_log_tree(fp, lnode->iface, &(lnode->bst));
		lnode = lnode->next;
	}
	fclose(fp);
}

void			ft_send_ip_stat(t_list **head, t_ip ip, FILE *fp)
{
	t_list	*node;

	fprintf(fp, "Stat for %hhu.%hhu.%hhu.%hhu:\n",
		ip.bytes[0],
		ip.bytes[1],
		ip.bytes[2],
		ip.bytes[3]);

	node = *head;
	while (node)
	{
		ft_send_ip_count(node->iface, &(node->bst), ip, fp);
		node = node->next;
	}
	fflush(fp);
}

void			ft_send_iface_stat(t_list **head, const char *iface, FILE *fp)
{
	t_list	*node;

	node = *head;
	while (node)
	{
		if (strcmp(iface, node->iface) == 0)
			break ;
		node = node->next;
	}
	if (node)
		ft_send_iface_info(iface, &(node->bst), fp);
	else
	{
		node = *head;
		while (node)
		{
			ft_send_iface_info(node->iface, &(node->bst), fp);
			node = node->next;
		}
	}
	fflush(fp);
}
