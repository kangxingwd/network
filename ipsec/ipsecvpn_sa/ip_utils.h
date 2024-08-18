#ifndef MYIP_UTILS_H
#define MYIP_UTILS_H

#include <stdio.h>
#include <arpa/inet.h>

#define IP2STR_4(addr_str, size, ip) { \
    do { \
        snprintf(addr_str, size, "%d.%d.%d.%d",  \
            (ip >> 24) & 0xff, \
            (ip >> 16) & 0xff, \
            (ip >> 8) & 0xff, \
            ip & 0xff); \
    } while(0); \
}

#define IPH_PRINT_IP(iph) { \
    do { \
        char saddr[17]; \
        char daddr[17]; \
        memset(saddr, 0, sizeof(saddr)); \
        memset(daddr, 0, sizeof(daddr)); \
        IP2STR_4(saddr, sizeof(saddr), ntohl(iph->src_addr)); \
        IP2STR_4(daddr, sizeof(daddr), ntohl(iph->dst_addr)); \
        printf("------------------------------  saddr: %s, daddr: %s\n", saddr, daddr); \
    } while(0); \
}

static void ipaddr_to_str(char *addr_str, int size, unsigned int ip)
{
    snprintf(addr_str, size, "%d.%d.%d.%d", 
        (ip >> 24) & 0xff,
        (ip >> 16) & 0xff,
        (ip >> 8) & 0xff,
        ip & 0xff);
}

static unsigned int ipv4_str_to_int(const char *ip_str) {
    struct in_addr ip_addr;
    if (inet_pton(AF_INET, ip_str, &ip_addr) != 1 ) {
        return 0;
    }
    return ntohl(ip_addr.s_addr);
}


static char* ipv4_str(unsigned int ip) {
    static char buf[64];

    if (inet_ntop(AF_INET, &ip, buf, 64) != NULL) {
        return (char*)buf;
    }
    strcpy(buf, "Invalid ip");
    return (char *)buf;
}

static char* ipv4_str2(unsigned int ip, int num) {
    static char buf1[64];
    static char buf2[64];

    if (num == 2) {
        if (inet_ntop(AF_INET, &ip, buf2, 64) != NULL) {
            return (char*)buf2;
        }
        strcpy(buf2, "Invalid ip");
        return (char *)buf2;
    } else {
        if (inet_ntop(AF_INET, &ip, buf1, 64) != NULL) {
            return (char*)buf1;
        }
        strcpy(buf1, "Invalid ip");
        return (char *)buf1;
    }
}

// struct in_addr ipv4_addr;
// ipv4_addr.s_addr = inet_addr("192.168.1.1");
// char ipv4_str[INET_ADDRSTRLEN];
// inet_ntop(AF_INET, &ipv4_addr, ipv4_str, INET_ADDRSTRLEN)

// struct in6_addr ipv6_addr;
// inet_pton(AF_INET6, "2001:0db8:85a3:0000:0000:8a2e:0370:7334", &ipv6_addr);
// char ipv6_str[INET6_ADDRSTRLEN];
// inet_ntop(AF_INET6, &ipv6_addr, ipv6_str, INET6_ADDRSTRLEN)


//  eth_type  arp 0x0806   ipv4 0x0800



#endif

