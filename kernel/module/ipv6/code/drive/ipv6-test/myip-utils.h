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

#define IP6_PRINT_IPADDR(addr) do { \
	printk(KERN_CONT "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x", \
		addr.in6_u.u6_addr8[0],	\
		addr.in6_u.u6_addr8[1],	\
		addr.in6_u.u6_addr8[2],	\
		addr.in6_u.u6_addr8[3],	\
		addr.in6_u.u6_addr8[4],	\
		addr.in6_u.u6_addr8[5],	\
		addr.in6_u.u6_addr8[6],	\
		addr.in6_u.u6_addr8[7],	\
		addr.in6_u.u6_addr8[8],	\
		addr.in6_u.u6_addr8[9],	\
		addr.in6_u.u6_addr8[10],	\
		addr.in6_u.u6_addr8[11],	\
		addr.in6_u.u6_addr8[12],	\
		addr.in6_u.u6_addr8[13],	\
		addr.in6_u.u6_addr8[14],	\
		addr.in6_u.u6_addr8[15]);	\
} while(0)

#define IP6_PRINT_IP(ip6h) do { \
	printk(KERN_CONT  "saddr: ");	\
	IP6_PRINT_IPADDR(ip6h->saddr);	\
	printk(KERN_CONT  " daddr: ");	\
	IP6_PRINT_IPADDR(ip6h->daddr);	\
	printk(KERN_CONT  "\n");	\
} while(0)

#endif