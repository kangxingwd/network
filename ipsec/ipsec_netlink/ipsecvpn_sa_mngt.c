#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/xfrm.h>

#include "ipsecvpn_sa_mngt.h"
#include "log.h"

int deal_broadcast_msg(struct ipsecvpn_sa_mngt_s* ipsecvpn_sa_mngt)
{
    IpsecSa *sa;
    char recv_buf[MAX_RECV_BUF];
    int msglen = 0;
    int has_do_sync = 0;
    bzero(recv_buf, MAX_RECV_BUF);

    if (!ipsecvpn_sa_mngt->listen_sock.data_is_ready())
        return has_do_sync;
    
    while((msglen = ipsecvpn_sa_mngt->listen_sock.recv_msg(recv_buf, MAX_RECV_BUF)) > 0)
    {
        struct nlmsghdr *n = (struct nlmsghdr *)recv_buf;
        while (NLMSG_OK(n, msglen))
        {
            sa = NULL;
            switch (n->nlmsg_type)
            {
                case NLMSG_DONE:
                    msg(DEBUG, "recv nlmsg_type = NLMSG_DONE return");
                    return has_do_sync;
                case NLMSG_ERROR:
                    msg(ERROR, "recv nlmsg_type = NLMSG_ERROR break");
                    return has_do_sync;
                case XFRM_MSG_NEWSA:
                case XFRM_MSG_UPDSA:
                    sa = update_sa(n);
                    break;
                case XFRM_MSG_DELSA:
                    sa = delete_sa(n);
                    break;
                case XFRM_MSG_NEWPOLICY:
                case XFRM_MSG_UPDPOLICY:
                    sa = update_policy(n);
                    break;
                case XFRM_MSG_DELPOLICY: //不处理删除策略的消息，因为在删sa的时候就一起删了
                    sa = delete_policy(n);
                    break;
                default:
                    LOG_ERROR("Not a state: %08x %08x %08x\n", n->nlmsg_len, n->nlmsg_type, n->nlmsg_flags);
                    break;
            }
            if (NULL != sa)
            {
                if (sa->m_state == IPSEC_STATE_OK || sa->m_state == IPSEC_STATE_NONE)
                {
                    has_do_sync = 1;
                    ipsecvpn_sa_mngt->need_switch = 1;
                }
                sa->m_seq_num = __sync_add_and_fetch(&ipsecvpn_sa_mngt->seq_num, 1);    // __sync_add_and_fetch 
            }
            n = NLMSG_NEXT(n, msglen);
        }
    }
    LOG_DEBUG("out deal_netlink_msg\n");
    return has_do_sync;
}

void ipsecvpn_sa_mngt_sync(struct ipsecvpn_sa_mngt_s* ipsecvpn_sa_mngt)
{
    bool need_switch =  false;
    if (ipsecvpn_sa_mngt->listen_sock.data_is_ready()) {
        need_switch = deal_broadcast_msg(ipsecvpn_sa_mngt);
        if (need_switch)
            LOG_DEBUG("do_sync because of broadcast\n");
    }
    else if (is_enforce_sync()) {
        need_switch = do_enforce_sync();
        if (need_switch)
            LOG_DEBUG("do switch because of enforce_sync\n");
        ipsecvpn_sa_mngt->last_sync_time = time(NULL);
    }

    if(need_switch) {
        ipsecvpn_sa_mngt->enforce_sync_interval = 30;
        ipsecvpn_sa_mngt->last_sync_time = time(NULL);
    }
}

int ipsecvpn_sa_mngt_init(struct ipsecvpn_sa_mngt_s* ipsecvpn_sa_mngt) 
{
    
    unsigned xfrm_groups = ((1<<(XFRMNLGRP_POLICY-1)) | (1<<(XFRMNLGRP_SA-1)));
    int ret = -1;

    ipsec_netlink_init(&ipsecvpn_sa_mngt->listen_sock);
    ipsec_netlink_init(&ipsecvpn_sa_mngt->sync_sock);

    ret = ipsecvpn_sa_mngt->listen_sock.open(ipsecvpn_sa_mngt->listen_sock, xfrm_groups, NETLINK_XFRM);
    if (ret < 0) {
        LOG_ERROR("netlink listen_sock open faild\n");
        return -1;
    }

    ret = -1;
    ret = ipsecvpn_sa_mngt->sync_sock.open(ipsecvpn_sa_mngt->listen_sock, 0, NETLINK_XFRM);
    if (ret < 0) {
        LOG_ERROR("netlink sync_sock open faild\n");
        return -1;
    }

    ipsec_lock_init(&ipsecvpn_sa_mngt->lock);

    ipsecvpn_sa_mngt->need_switch = 1;
    ipsecvpn_sa_mngt->is_running = 0;
    
    ipsecvpn_sa_mngt->enforce_sync = 1;
    ipsecvpn_sa_mngt->enforce_sync_interval = 30;
    ipsecvpn_sa_mngt->last_sync_time = time(NULL);

    ipsecvpn_sa_mngt->seq_num = 0;

    return 0;
}

int ipsecvpn_sa_mngt_desory(struct ipsecvpn_sa_mngt_s* ipsecvpn_sa_mngt)
{
    ipsec_netlink_destroy(&ipsecvpn_sa_mngt->listen_sock);
    ipsec_netlink_destroy(&ipsecvpn_sa_mngt->sync_sock);
    ipsec_lock_destroy(&ipsecvpn_sa_mngt->lock);
    
    return 0;
}
