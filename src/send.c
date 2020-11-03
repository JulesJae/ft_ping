#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>

#define IPV4_MINIMAL_MTU 64

void set_iphdr(char *packet, char *addr)
{
    struct iphdr *iph = (struct iphdr*)packet;

    iph->version = 4;
    iph->ihl = sizeof(*iph) >> 2;
    iph->ttl = 64;
    iph->protocol = IPPROTO_ICMP;
    inet_pton(AF_INET, addr, &iph->daddr);
    printf("l'adresse est egale a : %d\n", iph->daddr);
}

void set_icmphdr(char *packet)
{
    struct icmphdr *icmp = (struct icmphdr*)((struct iphdr*)packet + 1);

    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(getpid());
    icmp->un.echo.sequence = htons(1);
    printf("la taille du header icmp est: %ld\n", sizeof(*icmp));
    icmp->checksum = checksum((unsigned short*)icmp, sizeof(*icmp));
}

int open_raw_socket()
{
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
    {
        printf("\nSocket file descriptor not received!!\n"); 
        return 0; 
    }
    return sockfd;
}

void send_packet(char *packet, int sockfd, struct sockaddr_in *dest)
{
    int pid = getpid();

    printf("le pid = %d\n", pid);
    sendto(sockfd, (void*)packet, IPV4_MINIMAL_MTU, 0, (void*)dest, sizeof(struct sockaddr_in));
    recvfrom(sockfd, packet, IPV4_MINIMAL_MTU, 0, (struct sockaddr*)dest,sizeof(struct sockaddr_in));

}

int get_addresses(char *name, struct addrinfo **res)
{
  struct addrinfo hints;

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_flags = AI_CANONNAME;

    if (getaddrinfo(name, NULL, &hints, res) != 0)
        return 1;
    return (0);
}

struct sockaddr_in *get_recipient(char *addr)
{
    struct addrinfo *p;
    struct addrinfo *res;

    if (get_addresses(addr, &res))
    {
        printf("%s -> error to fetch address\n", addr);
        return NULL;
    }
    printf("IPADDR for %s(%s)\n", addr, res->ai_canonname);
    p = res;
    while (p) {

        if (p->ai_family == AF_INET)
        {
            return (struct sockaddr_in *)p->ai_addr;
        }
        p = p->ai_next;
    }
    return NULL;
}

int main(int ac, char *av[])
{
    char packet[IPV4_MINIMAL_MTU];
    struct sockaddr_in *dest;
    int sockfd;

    if (getuid() != 0)
    {
        printf("ft_ping: Operation not permitted\n");
        return (1);
    }
    dest = get_recipient(av[1]);
    bzero(&packet, IPV4_MINIMAL_MTU);
    set_iphdr(packet, av[1]);
    set_icmphdr(packet);
    printf("la taille total des deux en tetes est egales a %ld\n", sizeof(struct iphdr) + sizeof(struct icmphdr));
    sockfd = open_raw_socket();
    send_packet(packet, sockfd, dest);
    printf("la socket est egale a : %d\n", sockfd);
    return (0);
}
