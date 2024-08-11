#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/netlink.h>

#include "nl-default.h"
#include "netlink/netlink.h"
#include "netlink/socket.h"
#include "netlink/cache.h"
#include "netlink/xfrm/sa.h"

#include "log.h"


struct nl_cache_mngr *sa_cache_mngr = NULL;
struct nl_cache_mngr *sp_cache_mngr = NULL;
struct nl_sock *sa_sock;
struct nl_sock *sp_sock;
struct nl_cache *sa_ct;
struct nl_cache *sp_ct;
time_t last_sync_time;
int need_sync = 0;
int enforce_sync_interval = 30;
int cache_sync_thread_runing = 1;

static void sa_change_cb(struct nl_cache *cache, struct nl_object *obj,
		      int action, void *data)
{
	// struct nfnl_ct *ct = (struct nfnl_ct *) obj;
	// static struct nl_addr *hack = NULL;

	// if (!hack)
	// 	nl_addr_parse("194.88.212.233", AF_INET, &hack);

	// if (!nl_addr_cmp(hack, nfnl_ct_get_src(ct, 1)) ||
	//     !nl_addr_cmp(hack, nfnl_ct_get_dst(ct, 1))) {
	// 	struct nl_dump_params dp = {
	// 		.dp_type = NL_DUMP_LINE,
	// 		.dp_fd = stdout,
	// 	};

	// 	printf("UPDATE ");
	// 	nl_object_dump(obj, &dp);
	// }
    LOG_DEBUG_S("########### sa_change_cb \n");
}

static void sp_change_cb(struct nl_cache *cache, struct nl_object *obj,
		      int action, void *data)
{
	// struct nfnl_ct *ct = (struct nfnl_ct *) obj;
	// static struct nl_addr *hack = NULL;

	// if (!hack)
	// 	nl_addr_parse("194.88.212.233", AF_INET, &hack);

	// if (!nl_addr_cmp(hack, nfnl_ct_get_src(ct, 1)) ||
	//     !nl_addr_cmp(hack, nfnl_ct_get_dst(ct, 1))) {
	// 	struct nl_dump_params dp = {
	// 		.dp_type = NL_DUMP_LINE,
	// 		.dp_fd = stdout,
	// 	};

	// 	printf("UPDATE ");
	// 	nl_object_dump(obj, &dp);
	// }
    LOG_DEBUG_S("########### sp_change_cb \n");
}

void *ipsecvpn_cache_sync_from_kernel(void *arg) {
    LOG_DEBUG_S("This is the thread function.\n");
    int err;
    last_sync_time = time(NULL);

    while (cache_sync_thread_runing) {
		err = nl_cache_mngr_poll(sa_cache_mngr, 500);
		if (err < 0) {
			LOG_ERROR_S("nl_cache_mngr_poll() sa failed\n");
			return NULL;
		}

        err = nl_cache_mngr_poll(sp_cache_mngr, 500);
		if (err < 0) {
			LOG_ERROR_S("nl_cache_mngr_poll() sp failed\n");
			return NULL;
		}

        if((time(NULL) - last_sync_time) > enforce_sync_interval) {
            need_sync = 1;
        }

        if (need_sync) {
            LOG_DEBUG_S("sync sa sp ... \n");
            nl_cache_refill(sa_sock, sa_ct);
            nl_cache_refill(sp_sock, sp_ct);

            last_sync_time = time(NULL);
            need_sync = 0;
        }

	}

    return NULL;
}

int cache_init() 
{
    int err;

    sa_sock = nl_socket_alloc();
    err = nl_cache_mngr_alloc(sa_sock, NETLINK_XFRM, NL_AUTO_PROVIDE, &sa_cache_mngr);
    if (err < 0) {
		LOG_ERROR_S("nl_cache_mngr_alloc sa_sock failed\n");
		return -1;
	}
    err = nl_cache_mngr_add(sa_cache_mngr, "xfrm/sa", &sa_change_cb, NULL, &sa_ct);
	if (err < 0) {
		LOG_ERROR_S("nl_cache_mngr_add(xfrm/sa) failed\n");
		return -1;
	}

    sp_sock = nl_socket_alloc();
    err = nl_cache_mngr_alloc(sp_sock, NETLINK_XFRM, NL_AUTO_PROVIDE, &sp_cache_mngr);
    if (err < 0) {
		LOG_ERROR_S("nl_cache_mngr_alloc sp_sock failed\n");
		return -1;
	}
    err = nl_cache_mngr_add(sp_cache_mngr, "xfrm/sp", &sp_change_cb, NULL, &sp_ct);
	if (err < 0) {
		nl_perror(err, "nl_cache_mngr_add(xfrm/sp) failed\n");
		return -1;
	}

    return 0;
}

int cache_destory() 
{
    nl_cache_mngr_free(sa_cache_mngr);
    nl_cache_mngr_free(sp_cache_mngr);
}

void sigint_handler(int signum) {
    LOG_ERROR_S("Received SIGINT. Performing cleanup...\n");
    cache_destory();
    exit(0);
}

struct ttttt;

int main()
{
    int ret;
    pthread_t thread;

    LOG_INIT(LOG_MODE_STDOUT, LOG_LEVEL_DEBUG, NULL);
    LOG_DEBUG_S("main start ...\n");

    signal(SIGINT, sigint_handler);

    // struct xfrmnl_sa* a  = xfrmnl_sa_alloc();

    ret = cache_init();
    if (ret != 0) {
        LOG_ERROR_S("cache init failed!\n");
        return 1;
    }
    
    ret = pthread_create(&thread, NULL, ipsecvpn_cache_sync_from_kernel, NULL);
    if (ret != 0) {
        LOG_ERROR_S("pthread_create failed\n");
        return 1;
    }

    struct nl_dump_params params = {
		.dp_type = NL_DUMP_LINE,
		.dp_fd = stdout,
	};

    while (1) {

        LOG_DEBUG_S("########################## sa cache \n");
        // nl_cache_dump(sa_ct, &params);

        LOG_DEBUG_S("########################## sp cache \n");
        nl_cache_dump(sp_ct, &params);


        struct nl_addr* saddr;
        struct nl_addr* daddr;
        // nl_addr_parse("192.168.131.129", AF_INET, &daddr);

        // struct xfrmnl_sa *find_sa;
        // struct nl_addr* find_daddr;
        // nl_addr_parse("192.168.131.129", AF_INET, &daddr);
        // find_sa = xfrmnl_sa_get(sa_ct, daddr, 3231051399, 50);
        // if (find_sa) {
        //     LOG_DEBUG("find sa: spi:%u\n", xfrmnl_sa_get_spi(find_sa));
        // }

        
        // struct ttttt* tt;
        // tt = (struct ttttt*)nl_cache_get_first (sa_ct);
        // print("%s\n", tt->idd);

        
        // struct xfrmnl_sa *sa;
        // //nl_list_for_each_entry(sa, &cache->c_items, ce_list) {
        // for (sa = (struct xfrmnl_sa*)nl_cache_get_first (sa_ct);
        //     sa != NULL;
        //     sa = (struct xfrmnl_sa*)nl_cache_get_next ((struct nl_object*)sa))
        // {
        //     char *buf[64];
        //     daddr = xfrmnl_sa_get_daddr(sa);
        //     nl_addr2str(daddr, buf, 64);
        //     char* mode[64];
        //     xfrmnl_sa_mode2str(xfrmnl_sa_get_mode(sa), mode, 64);

        //     LOG_DEBUG("proto: %u, spi: %u, 0x%4x, daddr: %s, mode: %s\n", 
        //         xfrmnl_sa_get_proto(sa), xfrmnl_sa_get_spi(sa), xfrmnl_sa_get_spi(sa), buf, 
        //         mode);
        // }
        
        sleep(5);
    }

    cache_sync_thread_runing = 0;
    pthread_join(thread, NULL);
    cache_destory();

    return 0;
}

// test-nf-cache-mngr.c
