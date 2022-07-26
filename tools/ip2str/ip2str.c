#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// int inet_aton(const char *cp, struct in_addr *inp);
// char *inet_ntoa(struct in_addr in);

/*
    #include<arpa/inet.h>
    int inet_pton(int family, const char *strptr, void *addrptr);  // 返回：若成功则为1,若输入不是有效的表达格式则为0,若出错则为-1
    const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len); // 返回：若成功则为指向结果的指针， 若出错则为NULL
*/

#define INET_ADDRSTRLEN   16
#define INET6_ADDRSTRLEN  46

void help()
{
    printf("Usage: ip2str [ INET ] [ MODE ] STR\n");
    printf("    INET: 4|6 \n");
    printf("    MODE: 1|2  1[ip string to network byte], 2[network byte to ip string] \n");
    printf("    STR:  ip string or network byte\n");
    printf("\n");
}

#define IP6_PRINT_IPADDR(addr) do { \
	printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", \
		addr.s6_addr[0],	\
		addr.s6_addr[1],	\
		addr.s6_addr[2],	\
		addr.s6_addr[3],	\
		addr.s6_addr[4],	\
		addr.s6_addr[5],	\
		addr.s6_addr[6],	\
		addr.s6_addr[7],	\
		addr.s6_addr[8],	\
		addr.s6_addr[9],	\
		addr.s6_addr[10],	\
		addr.s6_addr[11],	\
		addr.s6_addr[12],	\
		addr.s6_addr[13],	\
		addr.s6_addr[14],	\
		addr.s6_addr[15]);	\
} while(0)

int main(int argc, char *argv[])
{
    if (argc < 2) {
        help();
        exit(1);
    }

    int inet = atoi(argv[1]);
    if (inet != 4 && inet != 6) {
        help();
        exit(1);
    }

    int mode = atoi(argv[2]);
    if (mode != 1 && mode != 2) {
        help();
        exit(1);
    }

    if (inet == 4) {
        if (mode == 1) {
            char *ip = argv[3];
            struct in_addr inp;
            if (inet_aton(ip, &inp) == 0) {
                printf("Invalid address: %s\n", ip);
                exit(1);
            }

            printf("be: 网络字节序， le: 主机字节序\n");
            printf("%s -> ip_be: %u(0x%x), ip_le:%u(0x%x) \n", 
                ip, 
                *(unsigned int*)&inp, 
                *(unsigned int*)&inp, 
                ntohl(*(unsigned int*)&inp), 
                ntohl(*(unsigned int*)&inp));

        } else {
            printf("be: 网络字节序， le: 主机字节序\n");
            unsigned int ip_be = (unsigned int)atol(argv[3]);
            printf("(ip_be)%s -> %s\n", argv[2], inet_ntoa(*(struct in_addr*)&ip_be));
            ip_be = htonl((unsigned int)atol(argv[3]));
            printf("(ip_le)%s -> %s\n", argv[2], inet_ntoa(*(struct in_addr*)&ip_be));
        }
    } else {
        if (mode == 1) {
            char *ip = argv[3];
            struct in6_addr sin6_addr;
            if (inet_pton(AF_INET6, ip, &sin6_addr) != 1) {
                printf("Invalid address: %s\n", ip);
                exit(1);
            }

            printf("%s -> ", ip);
            IP6_PRINT_IPADDR(sin6_addr);
            printf("\n");

        } else {
            printf("%s -> \n", argv[3]);
        }
    }
    return 0;
}

/*
echo "10.1.2.3" | awk -F'.' '{a=$1*2**24;b=$2*2**16;c=$3*2**8;d=$4;e=a+b+c+d;print e}'
167838211
echo "167838211" | awk '{a=0xFF000000;b=0xFF0000;c=0xFF00;d=0xFF;e=rshift(and($1,a),24);f=rshift(and($1,b),16);g=rshift(and($1,c),8);h=and($1,d);printf("%d.%d.%d.%d\n",e,f,g,h)}'
10.1.2.3

rshift 需要 gawk
apt-get install gawk 
*/

