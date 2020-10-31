#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <unistd.h>
# include <netinet/ip_icmp.h>
# include <netdb.h>
# include <time.h>
# include <string.h>
# include <arpa/inet.h>
# include <signal.h>

# define IPV4_MINIMAL_MTU 64

typedef struct      s_pckt
{
    char            buf[IPV4_MINIMAL_MTU];
    struct iphdr    *ip;
    struct icmphdr  *icmp;
}                   t_pckt;


unsigned short	    checksum(unsigned short *data, int len);

#endif