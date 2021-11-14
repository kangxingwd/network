#ifndef MYIP_UTILS_H
#define MYIP_UTILS_H

#include <linux/ip.h>

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
        IP2STR_4(saddr, sizeof(saddr), ntohl(iph->saddr)); \
        IP2STR_4(daddr, sizeof(daddr), ntohl(iph->daddr)); \
        printk("saddr: %s, daddr: %s\n", saddr, daddr); \
    } while(0); \
}

static void ipaddr_to_str(char *addr_str, int size, uint32_t ip)
{
    snprintf(addr_str, size, "%d.%d.%d.%d", 
        (ip >> 24) & 0xff,
        (ip >> 16) & 0xff,
        (ip >> 8) & 0xff,
        ip & 0xff);
}

#endif