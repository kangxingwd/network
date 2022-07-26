#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// int inet_aton(const char *cp, struct in_addr *inp);
// char *inet_ntoa(struct in_addr in);

void help()
{
    printf("Usage: ip2str [ MODE ] STR\n");
    printf("    MODE: 1|2  1[ip string to network byte], 2[network byte to ip string] \n");
    printf("    STR:  ip string or network byte\n");
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        help();
        exit(1);
    }

    int mode = atoi(argv[1]);
    if (mode != 1 && mode != 2) {
        help();
        exit(1);
    }

    if (mode == 1) {
        char *ip = argv[2];
        struct in_addr inp;
        if (inet_aton(ip, &inp) == 0) {
            printf("Invalid address: %s\n", ip);
            exit(1);
        }

        printf("be: 网络字节序， le: 主机字节序\n");
        printf("%s -> ip_be: %u, ip_le:%u \n", ip, *(unsigned int*)&inp, ntohl(*(unsigned int*)&inp));
    } else {
        printf("be: 网络字节序， le: 主机字节序\n");
        
        unsigned int ip_be = (unsigned int)atol(argv[2]);
        printf("(ip_be)%s -> %s\n", argv[2], inet_ntoa(*(struct in_addr*)&ip_be));

        ip_be = htonl((unsigned int)atol(argv[2]));
        printf("(ip_le)%s -> %s\n", argv[2], inet_ntoa(*(struct in_addr*)&ip_be));
    }

    return 0;
}
