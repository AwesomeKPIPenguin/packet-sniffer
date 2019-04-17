
#include <sys/mman.h>
#include "bst.h"

t_node			*ft_nodenew(t_ip ip)
{
	t_node	*node;

	node = (t_node *)malloc(sizeof(t_node));
	node->ip = ip;
	node->count = 1;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

void			ft_insert_ip(t_node **root, t_ip ip)
{
	int		is_left;
	t_node	*node;
	t_node	*parent;

	if (!root)
		return ;

	is_left = 0;
	node = *root;
	parent = node;
	while (node)
	{
		parent = node;
		if (node->ip.num == ip.num)
		{
			++(node->count);
			return ;
		}
		else if (node->ip.num > ip.num)
		{
			is_left = 1;
			node = node->left;
		}
		else
		{
			is_left = 0;
			node = node->right;
		}
	}
	if (!parent)
		*root = ft_nodenew(ip);
	else if (is_left)
		parent->left = ft_nodenew(ip);
	else
		parent->right = ft_nodenew(ip);
}

void			ft_insert_node(t_node **root, t_node *node)
{
	t_node	*tmp;
	t_node	*parent;

	if (!root)
		return ;

	tmp = *root;
	parent = tmp;
	while (tmp)
	{
		parent = tmp;
		if (tmp->ip.num > node->ip.num)
			tmp = tmp->left;
		else
			tmp = tmp->right;
	}
	if (!parent)
		*root = node;
	else if (parent->left == tmp)
		parent->left = node;
	else
		parent->right = node;
}

void			ft_send_ip_count(
					const char *iface, t_node **root, t_ip ip, int socket)
{
	t_node	*node;
	char	buffer[64];

	if (!root)
		return ;

	node = *root;
	while (node)
	{
		if (node->ip.num == ip.num)
		{
			bzero(buffer, 64);
			sprintf(buffer, "  %-8s: %-5d packets received\n",
				iface, node->count);
			send(socket, buffer, strlen(buffer), 0);
			return ;
		}
		else if (node->ip.num > ip.num)
			node = node->left;
		else
			node = node->right;
	}
}

static void		ft_send_formatted_info(t_node *node, int socket)
{
	char	buffer[64];

	bzero(buffer, 64);
	sprintf(buffer, "  %-3hhu.%-3hhu.%-3hhu.%-3hhu: %-6d packets received\n",
		node->ip.bytes[0],
		node->ip.bytes[1],
		node->ip.bytes[2],
		node->ip.bytes[3],
		node->count);
	send(socket, buffer, strlen(buffer), 0);
}

static void		ft_send_leaf_info(t_node *node, int socket)
{
	if (node->left)
		ft_send_leaf_info(node->left, socket);
	ft_send_formatted_info(node, socket);
	if (node->right)
		ft_send_leaf_info(node->right, socket);
}

void			ft_send_iface_info(const char *iface, t_node **root, int socket)
{
	if (!root)
		return ;

	send(socket, iface, strlen(iface), 0);
	send(socket, ":\n", 2, 0);
	ft_send_leaf_info(*root, socket);
}

void			ft_log_tree(FILE *fp, const char *iface, t_node **root)
{
	t_node	*node;

	node = *root;
	if (!node)
	{
		fflush(fp);
		return;
	}
	fprintf(fp, "%-14s:%-3hhu.%-3hhu.%-3hhu.%-3hhu:%-10d\n",
		iface,
		node->ip.bytes[0],
		node->ip.bytes[1],
		node->ip.bytes[2],
		node->ip.bytes[3],
		node->count);
	ft_log_tree(fp, iface, &(node->left));
	ft_log_tree(fp, iface, &(node->right));
}
