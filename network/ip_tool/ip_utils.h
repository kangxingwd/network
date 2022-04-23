
#ifndef __IP_UTILS_H__
#define __IP_UTILS_H__

typedef uint16_t __be16;
typedef uint32_t __be32;

union __l3_addr {
    uint32_t all[4];
    struct in6_addr in6;
};

struct l3_addr_st {
    union __l3_addr addr;
};

#define addr_ip addr.all[3]
#define addr_ip6 addr.all
#define addr_in6 addr.in6

/*
struct in6_addr {
	union {
		__u8		u6_addr8[16];
#if __UAPI_DEF_IN6_ADDR_ALT
		__be16		u6_addr16[8];
		__be32		u6_addr32[4];
#endif
	} in6_u;
#define s6_addr			in6_u.u6_addr8
#if __UAPI_DEF_IN6_ADDR_ALT
#define s6_addr16		in6_u.u6_addr16
#define s6_addr32		in6_u.u6_addr32
#endif
};
*/

static __inline int ipv4_addr_inc(struct l3_addr_st *l3_addr)
{
    if (l3_addr->addr_ip == (__be32)-1)
        return -1;
    
    l3_addr->addr_ip = htonl(ntohl(l3_addr->addr_ip) + 1);
    return 0;
}

static __inline int ipv6_addr_inc(struct l3_addr_st *l3_addr)
{
    int i;

    if (l3_addr->addr_ip6[0] == (__be32)-1 &&
        l3_addr->addr_ip6[1] == (__be32)-1 &&
        l3_addr->addr_ip6[2] == (__be32)-1 &&
        l3_addr->addr_ip6[3] == (__be32)-1)
            return -1;
    
    for (i = 3; i >=0; --i) {
        if ( (l3_addr->addr_ip6[i] = htonl(ntohl(l3_addr->addr_ip6[i]) + 1)) != 0 ) {
            break;
        }
    }
    return 0;
}

static __inline int ipv4_addr_dec(struct l3_addr_st *l3_addr)
{
    if (l3_addr->addr_ip == 0)
        return -1;
    
    l3_addr->addr_ip = htonl(ntohl(l3_addr->addr_ip) - 1);
    return 0;
}

static __inline int ipv6_addr_dec(struct l3_addr_st *l3_addr)
{
    int i;

    if (l3_addr->addr_ip6[0] == 0 &&
        l3_addr->addr_ip6[1] == 0 &&
        l3_addr->addr_ip6[2] == 0 &&
        l3_addr->addr_ip6[3] == 0)
            return -1;
    
    for (i = 3; i >=0; --i) {
        if ( (l3_addr->addr_ip6[i] = htonl(ntohl(l3_addr->addr_ip6[i]) - 1)) != (__be32)-1 ) {
            break;
        }
    }
    return 0;
}

#endif //__IP_UTILS_H__


