#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <poll.h>

// #include <linux/xfrm.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_link.h>
#include <linux/if_addr.h>
#include <linux/neighbour.h>

#include <rte_common.h>
#include <rte_hash.h>
#include <rte_jhash.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include "rte_malloc.h"
#include <rte_ring.h>
#include <rte_spinlock.h>

#include "libnl/netlink/socket.h"
#include "libnl/netlink/netlink.h"
#include "libnl/netlink/handlers.h"
#include "libnl/netlink/msg.h"

#include "ipsecvpn_sa.h"
#include "log.h"
#include "ip_utils.h"


static struct nl_sock *g_listen_sk = NULL;
static struct nl_sock *g_sync_sk = NULL;
static struct nl_cb *g_listen_cb = NULL;
static struct nl_cb *g_sync_cb = NULL;

static pthread_t g_sa_sync_thread;
static int g_sa_sync_thread_runing = 1;
static time_t g_last_sync_time;
static int g_need_sync = 0;
static int g_enforce_sync_interval = 30;

static pthread_t g_sa_clear_thread;
static int g_sa_clear_interval = 60;
static int g_sa_clear_thread_runing = 1;

static pthread_t g_sa_dump_thread;
static int g_sa_dump_interval = 30;
static int g_sa_dump_thread_runing = 1;

#define MAX_IPSEC_SA 4096
static struct rte_hash *g_sa_id_hash;
static struct rte_hash *g_sa_tunnel_hash;
static struct ipsec_sa_s* g_ipsec_sa_v;
static rte_spinlock_t g_sa_hash_lock;
static struct rte_ring* g_sa_free_index_ring;

static const char* sa_flags[256] = {
	[XFRM_STATE_NOECN]      = "no ecn",
	[XFRM_STATE_DECAP_DSCP] = "decap dscp",
	[XFRM_STATE_NOPMTUDISC] = "no pmtu discovery",
	[XFRM_STATE_WILDRECV]   = "wild receive",
	[XFRM_STATE_ICMP]       = "icmp",
	[XFRM_STATE_AF_UNSPEC]  = "unspecified",
	[XFRM_STATE_ALIGN4]     = "align4",
	[XFRM_STATE_ESN]        = "esn",
};

static const char* dir_flag[16] = {
	[XFRM_POLICY_IN]        = "in",
	[XFRM_POLICY_OUT]       = "out",
	[XFRM_POLICY_FWD]       = "forward",
	[XFRM_POLICY_MASK]      = "mask"
};

static char* hex_to_str(void *data, size_t size) {
    unsigned char *p = (unsigned char *)data;
    static char hex_str[4096] = {0};
    // char hex_str[size * 2 + 1];
    
    size_t d_size = (size > 2047)? 2047:size;
    hex_str[size * 2] = '\0';

    for (size_t i = 0; i < d_size; i++) {
        sprintf(&hex_str[i * 2], "%02x", p[i]);
    }

    return (char*)&hex_str;
}

static char* ipsec_sa_dump(struct ipsec_sa_s* ipsec_sa)
{
    static char sa_info[8192] = {0};
    char* ptr = sa_info;

    ptr += sprintf(ptr, "src %s dst %s\n", ipv4_str2(ipsec_sa->sa_info.saddr.a4, 1),ipv4_str2(ipsec_sa->sa_info.id.daddr.a4, 2));
    ptr += sprintf(ptr, "    proto %u spi 0x%x reqid %u model %s\n", 
        ipsec_sa->sa_info.id.proto, rte_be_to_cpu_32(ipsec_sa->sa_info.id.spi), ipsec_sa->sa_info.reqid,
        (ipsec_sa->sa_info.mode == XFRM_MODE_TRANSPORT)?"transport":"tunnel");
    ptr += sprintf(ptr, "    replay-window %u flag %s\n", ipsec_sa->sa_info.replay_window, sa_flags[ipsec_sa->sa_info.flags]);
    ptr += sprintf(ptr, "    auth-truc %s 0x%s bit:%u byte:%u\n", ipsec_sa->auth.alg_name, 
        hex_to_str(ipsec_sa->auth.alg_key, ipsec_sa->auth.alg_key_len), ipsec_sa->auth.alg_key_len*8, ipsec_sa->auth.alg_key_len);
    ptr += sprintf(ptr, "    enc %s 0x%s \n", ipsec_sa->crypt.alg_name, hex_to_str(ipsec_sa->crypt.alg_key, ipsec_sa->crypt.alg_key_len));
    ptr += sprintf(ptr, "    net:\n");
    ptr += sprintf(ptr, "        src %s/%u dst %s/%u \n", 
        ipv4_str2(ipsec_sa->policy_info.sel.saddr.a4, 1), ipsec_sa->policy_info.sel.prefixlen_s,
        ipv4_str2(ipsec_sa->policy_info.sel.daddr.a4, 2), ipsec_sa->policy_info.sel.prefixlen_d);
    ptr += sprintf(ptr, "        dir %s priority %u\n", dir_flag[ipsec_sa->policy_info.dir], ipsec_sa->policy_info.priority);
    sprintf(ptr, "\n");
    
    LOG_DEBUG("%s", sa_info);
    return sa_info;
}

void dump_all_ipsec_sa(void)
{
    struct ipsec_sa_s* ipsec_sa = NULL;
    int pos;
    uint32_t iter = 0;
    const void *next_key;
    FILE *fp = fopen("sa.txt", "w");
    if (fp == NULL) {
        LOG_ERROR_S("Error opening file\n");
        return;
    }

    rte_spinlock_lock(&g_sa_hash_lock);

    while ((pos = rte_hash_iterate(g_sa_id_hash, &next_key, (void **)&ipsec_sa, &iter)) >= 0) {
        if (ipsec_sa->state & IPSEC_STATE_OK && !ipsec_sa->del) {
            fputs(ipsec_sa_dump(ipsec_sa), fp);
        }
    }
    
    rte_spinlock_unlock(&g_sa_hash_lock);
    fclose(fp);
}

void *ipsecvpn_sa_dump(void *arg) {
    while (g_sa_dump_thread_runing) {        
        sleep(g_sa_dump_interval);
        dump_all_ipsec_sa();
    }
}

int sock_is_ready(struct nl_sock *nl_sk, int timeout)
{
    int ret;
    struct pollfd fds = {
		.fd = nl_socket_get_fd(nl_sk),
		.events = POLLIN,
	};

    ret = poll(&fds, 1, timeout);
    if (ret <= 0) {
		return 0;
	}
    return 1;
}

static struct nla_policy xfrm_sa_policy[XFRMA_MAX+1] = {
	[XFRMA_SA]              = { .minlen = sizeof(struct xfrm_usersa_info)},
	[XFRMA_ALG_AUTH_TRUNC]  = { .minlen = sizeof(struct xfrm_algo_auth)},
	[XFRMA_ALG_AEAD]        = { .minlen = sizeof(struct xfrm_algo_aead) },
	[XFRMA_ALG_AUTH]        = { .minlen = sizeof(struct xfrm_algo) },
	[XFRMA_ALG_CRYPT]       = { .minlen = sizeof(struct xfrm_algo) },
	[XFRMA_ALG_COMP]        = { .minlen = sizeof(struct xfrm_algo) },
	[XFRMA_ENCAP]           = { .minlen = sizeof(struct xfrm_encap_tmpl) },
	[XFRMA_TMPL]            = { .minlen = sizeof(struct xfrm_user_tmpl) },
	[XFRMA_SEC_CTX]         = { .minlen = sizeof(struct xfrm_sec_ctx) },
	[XFRMA_LTIME_VAL]       = { .minlen = sizeof(struct xfrm_lifetime_cur) },
	[XFRMA_REPLAY_VAL]      = { .minlen = sizeof(struct xfrm_replay_state) },
	// [XFRMA_OFFLOAD_DEV]     = { .minlen = sizeof(struct xfrm_user_offload) },
	[XFRMA_REPLAY_THRESH]   = { .type = NLA_U32 },
	[XFRMA_ETIMER_THRESH]   = { .type = NLA_U32 },
	[XFRMA_SRCADDR]         = { .minlen = sizeof(xfrm_address_t) },
	[XFRMA_COADDR]          = { .minlen = sizeof(xfrm_address_t) },
	[XFRMA_MARK]            = { .minlen = sizeof(struct xfrm_mark) },
	[XFRMA_TFCPAD]          = { .type = NLA_U32 },
	[XFRMA_REPLAY_ESN_VAL]  = { .minlen = sizeof(struct xfrm_replay_state_esn) },
};

struct ipsec_sa_s* get_free_sa(void)
{
    struct ipsec_sa_s* ipsec_sa = NULL;
    uint32_t index = 0;
    int ret;

    ret = rte_ring_sc_dequeue(g_sa_free_index_ring, (void *)&index);
    if (ret != 0) {
        LOG_ERROR_S("ipsec sa free index ring dequeue failed!\n");
        return NULL;
    }
    LOG_DEBUG("get free sa, index: %u\n", index);

    ipsec_sa = &g_ipsec_sa_v[index];
    memset((void*)ipsec_sa, 0, sizeof(struct ipsec_sa_s));
    ipsec_sa->used = 1;
    ipsec_sa->index = index;

    return ipsec_sa;
}

void delete_sa_from_kernel(int src_ip, int dst_ip, int proto, int spi)
{
    char cmd[128] = {0};
    int ret;
    snprintf(cmd, sizeof(cmd), "ip xfrm state delete src %s dst %s proto %s spi 0x%x", 
        ipv4_str2(src_ip, 1),  ipv4_str2(dst_ip, 2), (proto == IPPROTO_ESP)?"esp":"ah", ntohl(spi));
    ret = system(cmd);
    LOG_INFO("delete_sa_from_kernel, cmd:%s, ret:%d\n", cmd, ret);
}

struct ipsec_sa_s* ipsec_sa_find_by_id(struct xfrm_id *id)
{
    int ret;
    struct ipsec_sa_s* ipsec_sa = NULL;

    rte_spinlock_lock(&g_sa_hash_lock);

    ret = rte_hash_lookup_data(g_sa_id_hash, (void *)id, (void**)&ipsec_sa);
    if (ret >= 0 && ipsec_sa != NULL) {
        rte_atomic32_inc(&ipsec_sa->refcnt);
        rte_spinlock_unlock(&g_sa_hash_lock);
        return ipsec_sa;
    }

    rte_spinlock_unlock(&g_sa_hash_lock);

    return NULL;
}

struct ipsec_sa_s* ipsec_sa_find_by_tunnel(struct ipsec_tunnel_s *ipsec_tunnel)
{
    int ret;
    struct ipsec_sa_s* ipsec_sa = NULL;

    rte_spinlock_lock(&g_sa_hash_lock);

    ret = rte_hash_lookup_data(g_sa_tunnel_hash, (void *)ipsec_tunnel, (void**)&ipsec_sa);
    if (ret >= 0 && ipsec_sa != NULL) {
        rte_atomic32_inc(&ipsec_sa->refcnt);
        rte_spinlock_unlock(&g_sa_hash_lock);
        return ipsec_sa;
    }

    rte_spinlock_unlock(&g_sa_hash_lock);

    return NULL;
}

void ipsec_sa_put(struct ipsec_sa_s* ipsec_sa)
{
    if (ipsec_sa) {
        rte_atomic32_dec(&ipsec_sa->refcnt);
    }
}

void ipsec_sa_delete(struct ipsec_sa_s* ipsec_sa)
{
    struct ipsec_tunnel_s ipsec_tunnel;
    if (!ipsec_sa) {
        return;
    }

    ipsec_sa->del = 1;

    rte_spinlock_lock(&g_sa_hash_lock);
    
    if (rte_atomic32_read(&ipsec_sa->refcnt) <= 0) {
        ipsec_sa->used = 0;
        // TODO delete
        rte_hash_del_key(g_sa_id_hash, (void*)&ipsec_sa->sa_info.id);

        ipsec_tunnel.saddr = ipsec_sa->sa_info.saddr;
        ipsec_tunnel.daddr = ipsec_sa->sa_info.id.daddr;
        ipsec_tunnel.reqid = ipsec_sa->sa_info.reqid;
        rte_hash_del_key(g_sa_id_hash, (void*)&ipsec_tunnel);
  
        rte_ring_sp_enqueue(g_sa_free_index_ring, (void *)((uintptr_t)ipsec_sa->index));

        LOG_INFO("delete sa,  sip:%s, dip:%s, proto: %d, spi: 0x%x, reqid:%d \n", 
            ipv4_str2(ipsec_sa->sa_info.saddr.a4, 1),
            ipv4_str2(ipsec_sa->sa_info.id.daddr.a4, 2), 
            ipsec_sa->sa_info.id.proto,
            ipsec_sa->sa_info.id.spi,
            ipsec_sa->sa_info.reqid);
        
        memset((void*)ipsec_sa, 0, sizeof(struct ipsec_sa_s));
    }

    rte_spinlock_unlock(&g_sa_hash_lock);
}

void update_sa(struct nlmsghdr *n)
{
    struct nlattr               *tb[XFRMA_MAX + 1];
	struct xfrm_usersa_info*    sa_info;
    struct ipsec_sa_s           ipsec_sa;
    struct ipsec_sa_s*          new_ipsec_sa = NULL;
    struct ipsec_sa_s*          old_ipsec_sa = NULL;
    struct ipsec_tunnel_s ipsec_tunnel;
    int err, ret;
    char tunnel_str[1024];
	
    memset(&ipsec_sa, 0, sizeof(struct ipsec_sa_s));
    ipsec_sa.used = 1;

    sa_info = nlmsg_data(n);
    ipsec_tunnel.saddr = sa_info->saddr;
    ipsec_tunnel.daddr = sa_info->id.daddr;
    ipsec_tunnel.reqid = sa_info->reqid;
    sprintf(tunnel_str, "sip:%s, dip:%s, reqid:%u", ipv4_str2(ipsec_tunnel.saddr.a4, 1), ipv4_str2(ipsec_tunnel.daddr.a4, 1), ipsec_tunnel.reqid);

    err = nlmsg_parse(n, sizeof(struct xfrm_usersa_info), tb, XFRMA_MAX, xfrm_sa_policy);
	if (err < 0) {
        LOG_ERROR_S("update_sa nlmsg_parse failed!\n");
        return;
    }

    rte_memcpy(&ipsec_sa.sa_info, sa_info, sizeof(struct xfrm_usersa_info));

    // sa_info->mode,       XFRM_MODE_TRANSPORT  XFRM_MODE_TUNNEL
    // sa_info->id.proto,   IPPROTO_AH  IPPROTO_ESP

    if (tb[XFRMA_ALG_AEAD]) {
		struct xfrm_algo_aead* aead = nla_data(tb[XFRMA_ALG_AEAD]);

        rte_memcpy((void *)ipsec_sa.aead.alg_name, (void *)aead->alg_name, MAX_ALG_NAME);
        ipsec_sa.aead.alg_key_len = aead->alg_key_len / 8;
        ipsec_sa.aead.alg_icv_len = aead->alg_icv_len / 8;
        rte_memcpy((void *)ipsec_sa.aead.alg_key, (void *)aead->alg_key, ipsec_sa.aead.alg_key_len);
	}

    if (tb[XFRMA_ALG_AUTH_TRUNC]) {
		struct xfrm_algo_auth* auth = nla_data(tb[XFRMA_ALG_AUTH_TRUNC]);

		rte_memcpy((void *)ipsec_sa.auth.alg_name, (void *)auth->alg_name, MAX_ALG_NAME);
        ipsec_sa.auth.alg_key_len = auth->alg_key_len / 8;
        rte_memcpy((void *)ipsec_sa.auth.alg_key, (void *)auth->alg_key, ipsec_sa.auth.alg_key_len);
	}

    if (tb[XFRMA_ALG_AUTH] && !tb[XFRMA_ALG_AUTH_TRUNC]) {
		struct xfrm_algo* auth = nla_data(tb[XFRMA_ALG_AUTH]);

        rte_memcpy((void *)ipsec_sa.auth.alg_name, (void *)auth->alg_name, MAX_ALG_NAME);
        ipsec_sa.auth.alg_key_len = auth->alg_key_len / 8;
        rte_memcpy((void *)ipsec_sa.auth.alg_key, (void *)auth->alg_key, ipsec_sa.auth.alg_key_len);
	}

    if (tb[XFRMA_ALG_CRYPT]) {
		struct xfrm_algo* crypt = nla_data(tb[XFRMA_ALG_CRYPT]);

		rte_memcpy((void *)ipsec_sa.crypt.alg_name, (void *)crypt->alg_name, MAX_ALG_NAME);
        ipsec_sa.crypt.alg_key_len = crypt->alg_key_len / 8;
        rte_memcpy((void *)ipsec_sa.crypt.alg_key, (void *)crypt->alg_key, ipsec_sa.crypt.alg_key_len);
	}

    if (tb[XFRMA_ALG_COMP]) {
		struct xfrm_algo* comp = nla_data(tb[XFRMA_ALG_COMP]);

		rte_memcpy((void *)ipsec_sa.comp.alg_name, (void *)comp->alg_name, MAX_ALG_NAME);
        ipsec_sa.comp.alg_key_len = comp->alg_key_len / 8;
        rte_memcpy((void *)ipsec_sa.comp.alg_key, (void *)comp->alg_key, ipsec_sa.comp.alg_key_len);
	}

    if (tb[XFRMA_ENCAP]) {
		struct xfrm_encap_tmpl* encap = nla_data(tb[XFRMA_ENCAP]);

        rte_memcpy((void*)&ipsec_sa.encap_info, encap, sizeof (struct xfrm_encap_tmpl));
	}
    
    ipsec_sa.state |= IPSEC_STATE_SA_OK;

    // 查找是否存在
    old_ipsec_sa = ipsec_sa_find_by_tunnel(&ipsec_tunnel);
    if (old_ipsec_sa != NULL) {

        if (memcmp((void*)&(old_ipsec_sa->sa_info.id), (void*)&(ipsec_sa.sa_info.id), sizeof(struct xfrm_id)) == 0) {
            LOG_INFO_S("update sa, found sa is exist!\n");
            ipsec_sa_put(old_ipsec_sa);
            return;
        }

        if ( (ipsec_sa.state & IPSEC_STATE_SA_OK) && (old_ipsec_sa->sa_info.curlft.add_time >= ipsec_sa.sa_info.curlft.add_time)) {
            LOG_INFO_S("update sa, old sa add_time is new!\n");

            // TODO 删除内核旧的sa, ipsec_sa
            delete_sa_from_kernel((int)ipsec_sa.sa_info.saddr.a4, (int)ipsec_sa.sa_info.id.daddr.a4, (int)ipsec_sa.sa_info.id.proto, (int)ipsec_sa.sa_info.id.spi);
            ipsec_sa_put(old_ipsec_sa);
            return;
        }
        
        if (old_ipsec_sa->state & IPSEC_STATE_POLICY_OK) {
            rte_memcpy((void*)&ipsec_sa.policy_info, (void*)&old_ipsec_sa->policy_info, sizeof(struct xfrm_userpolicy_info));
            ipsec_sa.state |= IPSEC_STATE_POLICY_OK;
            old_ipsec_sa->state = IPSEC_STATE_NONE;
        }

        // 删除本地旧的SA
        ipsec_sa_put(old_ipsec_sa);
        ipsec_sa_delete(old_ipsec_sa);
        old_ipsec_sa = NULL;
    } else {
        LOG_INFO("no find sa by tunnel, add sa, ipsec_tunnel, sip:%s, dip:%s, reqid:%u\n", 
            ipv4_str2(ipsec_tunnel.saddr.a4, 1), ipv4_str2(ipsec_tunnel.daddr.a4, 2), ipsec_tunnel.reqid);
    }
    
    // get free node
    new_ipsec_sa = get_free_sa();
    if (!new_ipsec_sa) {
        LOG_ERROR_S("get_free_sa failed!\n");
        return;
    }

    rte_memcpy((void*)new_ipsec_sa, (void*)&ipsec_sa, sizeof(struct ipsec_sa_s));

    // insert id hash
    ret = rte_hash_add_key_data(g_sa_id_hash, (void*)&(new_ipsec_sa->sa_info.id), (void*)new_ipsec_sa);
    if (ret < 0) {
        LOG_ERROR("add hash failed! tunnel_str: %s\n", tunnel_str);
    }

    // insert tunnel hash
    ret = rte_hash_add_key_data(g_sa_tunnel_hash, (void*)&ipsec_tunnel, (void*)new_ipsec_sa);
    if (ret < 0) {
        LOG_ERROR("add hash failed! tunnel_str: %s\n", tunnel_str);
    }
    
    uint32_t id_sig = rte_hash_hash(g_sa_id_hash, (void*)&(new_ipsec_sa->sa_info.id));
    LOG_INFO("update sa,  sip:%s, dip:%s, proto: %d, spi: 0x%x, reqid:%d id_sig:%u, tunnel_sig:%u\n", 
            ipv4_str2(new_ipsec_sa->sa_info.saddr.a4, 1),
            ipv4_str2(new_ipsec_sa->sa_info.id.daddr.a4, 2), 
            new_ipsec_sa->sa_info.id.proto,
            new_ipsec_sa->sa_info.id.spi,
            new_ipsec_sa->sa_info.reqid);
}

static struct nla_policy xfrm_sp_policy[XFRMA_MAX+1] = {
	[XFRMA_POLICY]          = { .minlen = sizeof(struct xfrm_userpolicy_info)},
	[XFRMA_SEC_CTX]         = { .minlen = sizeof(struct xfrm_sec_ctx) },
	[XFRMA_TMPL]            = { .minlen = sizeof(struct xfrm_user_tmpl) },
	[XFRMA_POLICY_TYPE]     = { .minlen = sizeof(struct xfrm_userpolicy_type)},
	[XFRMA_MARK]            = { .minlen = sizeof(struct xfrm_mark) },
};

void update_policy(struct nlmsghdr *n)
{
    struct nlattr               *tb[XFRMA_MAX + 1];
	struct xfrm_userpolicy_info     *sp_info;
    int err, ret;
    char tunnel_str[1024];

    sp_info = nlmsg_data(n);
    err = nlmsg_parse(n, sizeof(struct xfrm_userpolicy_info), tb, XFRMA_MAX, xfrm_sp_policy);
	if (err < 0) {
        LOG_ERROR_S("update_policy nlmsg_parse failed!\n");
        return;
    }

    if (sp_info->dir != XFRM_POLICY_OUT && sp_info->dir != XFRM_POLICY_IN) {
        LOG_DEBUG("sp info dir is not XFRM_POLICY_OUT or XFRM_POLICY_IN, dir: %u\n", sp_info->dir);
        return;
    }

    if (tb[XFRMA_TMPL]) {
        struct xfrm_user_tmpl* tmpls = nla_data(tb[XFRMA_TMPL]);
		uint32_t i;
		uint32_t num_tmpls = nla_len(tb[XFRMA_TMPL]) / sizeof (*tmpls);

        for (i = 0; (i < num_tmpls) && (tmpls); i ++, tmpls++)
		{
            struct xfrm_user_tmpl *tmpl = &tmpls[i];

            if (tmpl->mode == XFRM_MODE_TRANSPORT) {
                tmpl->saddr = sp_info->sel.saddr;
                tmpl->id.daddr = sp_info->sel.daddr;
            }

            struct ipsec_tunnel_s tunnel = {
                .saddr = tmpl->saddr, 
                .daddr = tmpl->id.daddr, 
                .reqid = tmpl->reqid
            };
            sprintf(tunnel_str, "sip:%s, dip:%s, reqid:%u", ipv4_str2(tunnel.saddr.a4, 1), ipv4_str2(tunnel.daddr.a4, 1), tunnel.reqid);

            struct ipsec_sa_s *ipsec_sa = NULL;
            ipsec_sa = ipsec_sa_find_by_tunnel(&tunnel);
            if (ipsec_sa) {
                LOG_DEBUG("update_policy, found sa, tunnel_str: %s\n", tunnel_str);
            } else {
                LOG_DEBUG("update_policy, no found sa, tunnel_str: %s\n", tunnel_str);
                ipsec_sa = get_free_sa();
                if (!ipsec_sa) {
                    LOG_ERROR_S("update_policy, get_free_sa failed!\n");
                    continue;
                }
                // // insert id hash
                // rte_hash_add_key_data(g_sa_id_hash, (void*)&(new_ipsec_sa->sa_info.id), (void*)new_ipsec_sa);

                // insert tunnel hash
                ret = rte_hash_add_key_data(g_sa_tunnel_hash, (void*)&tunnel, (void*)ipsec_sa);
                if (ret < 0) {
                    LOG_ERROR("update_policy, add hash failed! tunnel_str: %s\n", tunnel_str);
                }
            }

            if (ipsec_sa->state & IPSEC_STATE_SA_OK) {
                LOG_INFO("add policy to sa, sa spi: 0x%x tunnel_str:%s\n", ipsec_sa->sa_info.id.spi, tunnel_str);
            }

            rte_memcpy((void*)&(ipsec_sa->policy_info), (void*)sp_info, sizeof(struct xfrm_userpolicy_info));
            ipsec_sa->state |= IPSEC_STATE_POLICY_OK;
            LOG_INFO("update policy, src_net:%s/%d, dst_net:%s/%d, reqid:%d, dir:%s \n", 
                ipv4_str2(sp_info->sel.saddr.a4, 1), sp_info->sel.prefixlen_s,
                ipv4_str2(sp_info->sel.daddr.a4, 2), sp_info->sel.prefixlen_d,
                tmpl->reqid, (sp_info->dir== XFRM_POLICY_IN) ? "in" : "out");
            
            ipsec_sa_put(ipsec_sa);
		}
    }
}

void deal_expire(struct nlmsghdr *n)
{
    struct xfrm_user_expire* ue;
    struct xfrm_usersa_info* sa_info;

    ue = nlmsg_data(n);
    sa_info = &ue->state;

    LOG_INFO("deal_expire,  sip:%s, dip:%s, proto: %d, spi: 0x%x, reqid:%d \n", 
            ipv4_str2(sa_info->saddr.a4, 1),
            ipv4_str2(sa_info->id.daddr.a4, 2), 
            sa_info->id.proto,
            sa_info->id.spi,
            sa_info->reqid);
}

static int sync_msg_parser(struct nl_msg *msg, void *arg)
{
    struct nlmsghdr *n = nlmsg_hdr(msg);
    LOG_DEBUG_S("######################### sync_msg_parser #########################\n");
    switch (n->nlmsg_type)
    {
        case XFRM_MSG_NEWSA:
            LOG_DEBUG("sync_msg_parser, recv XFRM_MSG_NEWSA, nlmsg_type = %d\n", n->nlmsg_type);
            update_sa(n);
            break;
        case XFRM_MSG_NEWPOLICY:
            LOG_DEBUG("sync_msg_parser, recv XFRM_MSG_NEWPOLICY, nlmsg_type = %d\n", n->nlmsg_type);
            update_policy(n);
            break;
        case XFRM_MSG_EXPIRE:
            LOG_DEBUG("sync_msg_parser, recv XFRM_MSG_EXPIRE, nlmsg_type = %d\n", n->nlmsg_type);
            deal_expire(n);
            break;
        default:
            LOG_DEBUG("sync_msg_parser, recv nlmsg_type = %d\n", n->nlmsg_type);
            break;
    }

	return NL_OK;
}

void delete_sa(struct nlmsghdr *n)
{
    struct xfrm_usersa_id *xsid = NULL;
    struct xfrm_id id;
    struct ipsec_sa_s* ipsec_sa = NULL;

    xsid = (struct xfrm_usersa_id *)nlmsg_data(n);

    id.daddr.a4 = xsid->daddr.a4;
    id.spi = xsid->spi;
    id.proto = xsid->proto;

    ipsec_sa = ipsec_sa_find_by_id(&id);
    if (ipsec_sa) {
        ipsec_sa->state &= (~IPSEC_STATE_SA_OK);
        if (ipsec_sa->sa_info.mode == XFRM_MODE_TRANSPORT) {
            ipsec_sa->state = IPSEC_STATE_NONE;
        }
        ipsec_sa_put(ipsec_sa);
        ipsec_sa_delete(ipsec_sa);
    }
}

void delete_policy(struct nlmsghdr *n)
{
    struct xfrm_userpolicy_id *xsid = NULL;
    struct ipsec_sa_s* ipsec_sa = NULL;
    int pos;
    uint32_t iter = 0;
    const void *next_key;

    xsid = (struct xfrm_userpolicy_id *)nlmsg_data(n);
    if (xsid->dir != XFRM_POLICY_OUT && xsid->dir != XFRM_POLICY_IN) {
        return;
    }

    rte_spinlock_lock(&g_sa_hash_lock);

    while ((pos = rte_hash_iterate(g_sa_id_hash, &next_key, (void **)&ipsec_sa, &iter)) >= 0) {
        if ( (ipsec_sa->policy_info.sel.saddr.a4 == xsid->sel.saddr.a4) && 
             (ipsec_sa->policy_info.sel.daddr.a4 == xsid->sel.daddr.a4) && 
             (ipsec_sa->policy_info.dir == xsid->dir) &&  
             (ipsec_sa->sa_info.sel.prefixlen_s == xsid->sel.prefixlen_s) &&
             (ipsec_sa->sa_info.sel.prefixlen_d == xsid->sel.prefixlen_d) )  {

                ipsec_sa->state &= (~IPSEC_STATE_POLICY_OK);
                ipsec_sa->del = 1;
                break;
             }
    }
    
    rte_spinlock_unlock(&g_sa_hash_lock);
}

static int broadcast_msg_parser(struct nl_msg *msg, void *arg)
{
	// struct update_xdata *x = arg;
    struct nlmsghdr *n = nlmsg_hdr(msg);
	int ret = 0;
    LOG_DEBUG_S("######################### broadcast_msg_parser #########################\n");
    switch (n->nlmsg_type)
    {
        case XFRM_MSG_NEWSA:
        case XFRM_MSG_UPDSA:
            LOG_DEBUG("broadcast_msg_parser, recv XFRM_MSG_NEWSA, nlmsg_type = %d", n->nlmsg_type);
            update_sa(n);
            break;
        case XFRM_MSG_DELSA:
            LOG_DEBUG("broadcast_msg_parser, recv XFRM_MSG_DELSA, nlmsg_type = %d", n->nlmsg_type);
            delete_sa(n);
            break;
        case XFRM_MSG_NEWPOLICY:
        case XFRM_MSG_UPDPOLICY:
            LOG_DEBUG("broadcast_msg_parser, recv XFRM_MSG_NEWPOLICY, nlmsg_type = %d", n->nlmsg_type);
            update_policy(n);
            break;
        case XFRM_MSG_DELPOLICY: 
            LOG_DEBUG("broadcast_msg_parser, recv XFRM_MSG_DELPOLICY, nlmsg_type = %d", n->nlmsg_type);
            delete_policy(n);
            break;
        default:
            LOG_DEBUG("broadcast_msg_parser, recv nlmsg_type = %d", n->nlmsg_type);
            break;
    }

	return NL_OK;
}

void handle_update_msg(void)
{
    int err;
    err = nl_recvmsgs(g_sync_sk, g_sync_cb);
	if (err < 0) {
        LOG_ERROR("handle_update_msg, nl_recvmsgs failed! recvmsgs() returned %d: %s\n", err, nl_geterror(err));
        return;
    }
}

void handle_broadcast_msg(void)
{
    int err;
    err = nl_recvmsgs(g_listen_sk, g_listen_cb);
	if (err < 0) {
        LOG_ERROR("handle_broadcast_msg, nl_recvmsgs failed! recvmsgs() returned %d: %s\n", err, nl_geterror(err));
        return;
    }
}

void *ipsecvpn_sa_clear(void *arg) {
    LOG_DEBUG_S("This is the thread: ipsecvpn_sa_clear\n");
    uint32_t iter = 0;
    const void *next_key;
    struct ipsec_sa_s* ipsec_sa = NULL;
    struct ipsec_sa_s* del_sa[64];
    int pos; 


    int del_num = 0;
    int i = 0;

    while (g_sa_clear_thread_runing) {        
        sleep(g_sa_clear_interval);

        iter = 0;
        del_num = 0;

        while ((pos = rte_hash_iterate(g_sa_id_hash, &next_key, (void **)&ipsec_sa, &iter)) >= 0) {
            if (rte_atomic32_read(&ipsec_sa->refcnt) == 0 && ipsec_sa->del) {
                del_sa[del_num++] = ipsec_sa;

                if (del_num >= 64 ) {
                    break;
                }
            }
        }

        for (i = 0; i < del_num; i++) {
            LOG_INFO_S("ipsecvpn_sa_clear ... \n");
            ipsec_sa_delete(del_sa[i]);
        }
    }
}

void *ipsecvpn_cache_sync_from_kernel(void *arg) {
    LOG_DEBUG_S("This is the thread: ipsecvpn_cache_sync_from_kernel\n");
    int ret;
    g_last_sync_time = time(NULL);

    while (g_sa_sync_thread_runing) {
        if (sock_is_ready(g_listen_sk, 1000)) {
            handle_broadcast_msg();
        }

        if((time(NULL) - g_last_sync_time) > g_enforce_sync_interval) {
            g_need_sync = 1;
        }

        if (g_need_sync) {
            LOG_INFO_S("############################################################# sync all sa sp ... \n");
            
            ret = nl_send_simple(g_sync_sk, XFRM_MSG_GETSA, NLM_F_DUMP, NULL, 0);
            if (ret < 0) {
                LOG_ERROR_S("send getsa netlink request failed!\n");
                continue;
            }
            handle_update_msg();

            LOG_DEBUG_S("handle_update_msg get sa OK!\n");


            ret = nl_send_simple(g_sync_sk, XFRM_MSG_GETPOLICY, NLM_F_DUMP, NULL, 0);
            if (ret < 0) {
                LOG_ERROR_S("send getsp netlink request failed!\n");
                continue;
            }
            handle_update_msg();

            LOG_DEBUG_S("handle_update_msg get sp OK!\n");

            // TODO 清理sa sp


            g_last_sync_time = time(NULL);
            g_need_sync = 0;
        }

	}

    return NULL;
}

int sa_sync_sock_init(void)
{
    int ret;
    
    g_listen_sk = nl_socket_alloc();
    ret = nl_connect(g_listen_sk, NETLINK_XFRM);
	if (ret < 0) {
		LOG_ERROR_S("g_listen_sk nl_socket_alloc failed!\n");
        return -1;
	}
    ret = nl_socket_add_membership(g_listen_sk, XFRMNLGRP_SA);
    if (ret != 0) {
        LOG_ERROR("g_listen_sk nl_socket_add_memberships XFRMNLGRP_SA failed! %s\n",  nl_strerror_l(errno));
        return -1;
    }
    ret = nl_socket_add_membership(g_listen_sk, XFRMNLGRP_EXPIRE);
    if (ret != 0) {
        LOG_ERROR("g_listen_sk nl_socket_add_memberships XFRMNLGRP_EXPIRE failed! %s\n",  nl_strerror_l(errno));
        return -1;
    }
    ret = nl_socket_add_membership(g_listen_sk,XFRMNLGRP_POLICY);
    if (ret != 0) {
        LOG_ERROR("g_listen_sk nl_socket_add_memberships XFRMNLGRP_POLICY failed! %s\n",  nl_strerror_l(errno));
        return -1;
    }

    g_sync_sk = nl_socket_alloc();
    ret = nl_connect(g_sync_sk, NETLINK_XFRM);
	if (ret < 0) {
		LOG_ERROR_S("g_listen_sk nl_socket_alloc failed!\n");
        return -1;
	}


    g_sync_cb = nl_cb_alloc(NL_CB_CUSTOM);
    if (g_sync_cb == NULL) {
        LOG_ERROR_S("g_sync_cb nl_cb_alloc failed!\n");
        return -1;
    }
    nl_cb_set(g_sync_cb, NL_CB_VALID, NL_CB_CUSTOM, sync_msg_parser, NULL);

    g_listen_cb = nl_cb_alloc(NL_CB_CUSTOM);
    if (g_listen_cb == NULL) {
        LOG_ERROR_S("g_listen_cb nl_cb_alloc failed!\n");
        return -1;
    }
    nl_cb_set(g_listen_cb, NL_CB_VALID, NL_CB_CUSTOM, broadcast_msg_parser, NULL);

    return 0;
}

int sa_sync_sock_destroty(void)
{
    if (g_listen_sk) {
        nl_socket_free(g_listen_sk);
    }
    if (g_sync_sk) {
        nl_socket_free(g_sync_sk);
    }
    if (g_listen_cb) {
        nl_cb_put(g_listen_cb);
    }
    if (g_sync_cb) {
        nl_cb_put(g_sync_cb);
    }
    
    return 0;
}

int sa_hash_init(void)
{
    uint32_t i = 0;
    unsigned socket_id = rte_socket_id();
    struct rte_hash_parameters id_hash_params = {
        .name = "sa_id_hash",
		.entries = MAX_IPSEC_SA, /* table load = 50% */
		.key_len = sizeof(struct xfrm_id),
        .hash_func = rte_jhash,
		.socket_id = socket_id,
		.hash_func_init_val = 0,
	};
    struct rte_hash_parameters tunnel_hash_params = {
        .name = "sa_tunnel_hash",
		.entries = MAX_IPSEC_SA, /* table load = 50% */
		.key_len = sizeof(struct ipsec_tunnel_s),
        .hash_func = rte_jhash,
		.socket_id = socket_id,
		.hash_func_init_val = 0,
	};

    g_sa_id_hash = rte_hash_create(&id_hash_params);
    if (g_sa_id_hash == NULL) {
        LOG_ERROR_S("sa id hash creat failed!\n");
        goto err;
    }

    g_sa_tunnel_hash = rte_hash_create(&tunnel_hash_params);
    if (g_sa_tunnel_hash == NULL) {
        LOG_ERROR_S("sa tunnel hash creat failed!\n");
        goto err;
    }

    rte_spinlock_init(&g_sa_hash_lock);

    // 初始化 ipsec_sa 数组
    g_ipsec_sa_v = (struct ipsec_sa_s*)rte_zmalloc_socket(NULL, MAX_IPSEC_SA * sizeof(struct ipsec_sa_s), RTE_CACHE_LINE_SIZE, socket_id);
    memset(g_ipsec_sa_v, 0, MAX_IPSEC_SA * sizeof(struct ipsec_sa_s));

    g_sa_free_index_ring = rte_ring_create("sa_free_index_ring", rte_align32pow2(MAX_IPSEC_SA - 1), socket_id, 0);
    if (!g_sa_free_index_ring) {
        LOG_ERROR_S("sa_free_index_ring creat failed!\n");
        goto err;
    }
    for (i = 0; i < MAX_IPSEC_SA; i++) {
        rte_ring_sp_enqueue(g_sa_free_index_ring, (void *)((uintptr_t)i));
    }

    return 0;
err:
    if (g_sa_id_hash) {
        rte_hash_free(g_sa_id_hash);
    }

    if (g_sa_tunnel_hash) {
        rte_hash_free(g_sa_tunnel_hash);
    }

    if (g_sa_free_index_ring) {
        rte_ring_free(g_sa_free_index_ring);
    }
    return 1;
}

int ipsecvpn_sa_init(void)
{
    int ret;

    ret = sa_sync_sock_init();
    if (ret != 0) {
        LOG_ERROR_S("sa_sync_sock_init failed!\n");
        return 1;
    }

    ret = sa_hash_init();
    if (ret != 0) {
        LOG_ERROR_S("sa_hash_init failed!\n");
        return 1;
    }

    ret = pthread_create(&g_sa_sync_thread, NULL, ipsecvpn_cache_sync_from_kernel, NULL);
    if (ret != 0) {
        LOG_ERROR_S("pthread_create failed\n");
        return 1;
    }

    ret = pthread_create(&g_sa_clear_thread, NULL, ipsecvpn_sa_clear, NULL);
    if (ret != 0) {
        LOG_ERROR_S("pthread_create failed\n");
        return 1;
    }

    ret = pthread_create(&g_sa_dump_thread, NULL, ipsecvpn_sa_dump, NULL);
    if (ret != 0) {
        LOG_ERROR_S("pthread_create failed\n");
        return 1;
    }

    return 0;
}

int ipsecvpn_sa_destroty(void)
{
    g_sa_sync_thread_runing = 0;
    g_sa_clear_thread_runing = 0;
    g_sa_dump_thread_runing = 0;
    pthread_join(g_sa_sync_thread, NULL);
    pthread_join(g_sa_clear_thread, NULL);
    pthread_join(g_sa_dump_thread, NULL);

    sa_sync_sock_destroty();

    return 0;
}
