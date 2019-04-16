
#ifndef CLI_H

# define CLI_H

# include "sniff.h"

# include <memory.h>
# include <ctype.h>

# define BUFF_SIZE	128

void		ft_listen_for_commands(int socket);
void		ft_start(int socket);
void		ft_stop(int socket);
void		ft_get_ip_stat(int socket, t_ip ip);
void		ft_select_iface(
				int socket, const char *iface, char (*iface_ptr)[IFACE_SIZE]);
void		ft_iface_stat(int socket, const char *iface);

t_ip		ft_get_ip(const char *command);

#endif /* CLI_H */
