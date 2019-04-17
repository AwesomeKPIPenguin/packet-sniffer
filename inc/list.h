
#ifndef LIST_H

# define LIST_H

# include "bst.h"
# include <strings.h>
# include <memory.h>

# define IFACE_SIZE	14

typedef struct		s_list
{
	char			iface[IFACE_SIZE];
	t_node			*bst;
	struct s_list	*next;
}					t_list;

void				ft_init_from_file(t_list **head, FILE *fp);
t_list				*ft_search_iface(t_list **head, const char *iface);
void				ft_write_log(const char *filename, t_list **head);
void				ft_send_ip_stat(t_list **head, t_ip ip, FILE *fp);
void				ft_send_iface_stat(
						t_list **head, const char *iface, FILE *fp);

#endif /* LIST_H */
