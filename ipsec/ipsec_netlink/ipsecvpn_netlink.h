#ifndef __IPSECVPN_NETLINK_H_
#define __IPSECVPN_NETLINK_H_

#include "libnetlink.h"

struct ipsec_netlink_s {
    struct rtnl_handle rth;

    int (*open)(struct ipsec_netlink_s* ipsec_netlink, unsigned int subscriptions, int protocol);
    int (*listen)(struct ipsec_netlink_s* ipsec_netlink);
    int (*data_is_ready)(struct ipsec_netlink_s* ipsec_netlink);
    int (*send_req)(struct ipsec_netlink_s* ipsec_netlink, int type);
    int (*recv_msg)(struct ipsec_netlink_s* ipsec_netlink, char *buf, int len);

};

int ipsec_netlink_init(struct ipsec_netlink_s* ipsec_netlink);
int ipsec_netlink_destroy(struct ipsec_netlink_s* ipsec_netlink);

#endif

