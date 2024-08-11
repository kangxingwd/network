
#include "ipsecvpn_sa_mngt.h"
#include "time.h"

struct ipsecvpn_sa_mngt_s g_ipsecvpn_sa_mngt;

void *sync_sa_thread(void *arg)
{
    static int retval = 0;
    
    ipsecvpn_sa_mngt_init(&g_ipsecvpn_sa_mngt);

    while(1) {
        // ipsec::instance().check_sa();
        // IpsecSaManager::instance().do_sync();
        sleep(1);
    }

    return (void*)&retval;
}

