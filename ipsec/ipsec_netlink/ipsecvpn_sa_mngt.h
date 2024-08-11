#ifndef __IPSECVPN_SA_MNGT_H_
#define __IPSECVPN_SA_MNGT_H_

#include <time.h>

#include "ipsecvpn_netlink.h"
#include "ipsecvpn_lock.h"


struct ipsecvpn_sa_mngt_s {
    struct ipsec_netlink_s listen_sock;   // 侦听netlink广播包套接字
    struct ipsec_netlink_s sync_sock;     // 用来获取sa和policy的套接字
    struct ipsecvpn_lock_s lock;

    int need_switch;
    int is_running;

    time_t last_sync_time;
    int enforce_sync;
    int enforce_sync_interval;
    
    unsigned int seq_num;
    
    
};

int ipsecvpn_sa_mngt_init(struct ipsecvpn_sa_mngt_s* ipsecvpn_sa_mngt);
void ipsecvpn_sa_mngt_sync(struct ipsecvpn_sa_mngt_s* ipsecvpn_sa_mngt); 

#endif

