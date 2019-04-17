
#include "utils.h"

t_ip			ft_parse_ip(const char *s, char **endptr)
{
	t_ip		ip;
	int32_t		block;
	char		*ptr;

	ip.num = 0;
	block = 0;
	ptr = NULL;

	/* minimal size of string, containing ip_for_stat */
	if (strlen(s) < 7)
		return ((t_ip)0u);

	for (int i = 0; i < 4; ++i)
	{
		block = (int32_t)strtol(s, &ptr, 10);
		if (block < 0 || block > 255)
			return ((t_ip)0u);
		ip.bytes[i] = (uint8_t)block;
		while (ptr && isspace(*ptr))
			++ptr;
		if (i != 3)
		{
			if (!ptr || ptr[0] != '.' || !ptr[1])
				return ((t_ip)0u);
			s = &(ptr[1]);
		}
	}
	if (endptr)
		*endptr = ptr;
	return (ip);
}
