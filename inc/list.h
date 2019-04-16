
#ifndef LIST_H

# define LIST_H

# include "bst.h"
# include <strings.h>
# include <memory.h>

# define IFACE_SIZE	10

typedef struct		s_list
{
	char			iface[IFACE_SIZE];
	t_node			*bst;
	struct s_list	*next;
}					t_list;

void				ft_init_from_file(t_list **head, FILE *fp);
t_list				*ft_search_iface(t_list **head, const char *iface);
void				ft_write_log(const char *filename, t_list **head);

#endif /* LIST_H */
