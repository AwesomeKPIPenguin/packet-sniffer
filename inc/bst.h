
#ifndef BST_H

# define BST_H

# include "utils.h"
# include <stdio.h>
# include <sys/socket.h>

# define LOG_LINE_SIZE	42
/* log file line structure: */
/*   14   1 15 1  10    1   */
/* <iface>:<ip>:<count>\n   */

typedef struct		s_node
{
	t_ip			ip;
	int				count;
	struct s_node	*left;
	struct s_node	*right;
}					t_node;

t_node				*ft_nodenew(t_ip ip);
void				ft_insert_ip(t_node **root, t_ip ip);
void				ft_insert_node(t_node **root, t_node *node);
void				ft_log_tree(FILE *fp, const char *iface, t_node **root);

#endif /* BST_H */
