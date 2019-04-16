
#ifndef UTILS_H

# define UTILS_H

# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>

typedef union		u_ip
{
	uint8_t			bytes[4];
	uint32_t		num;
}					t_ip;

t_ip				ft_parse_ip(const char *s, char **endptr);

#endif /* UTILS_H */
