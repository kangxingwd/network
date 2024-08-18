#ifndef __IPSECVPN_SA_H__
#define __IPSECVPN_SA_H__

#include <linux/xfrm.h>
#include <stdint.h>

#include <rte_atomic.h>

enum {
    IPSEC_STATE_NONE        =       0,
    IPSEC_STATE_SA_OK       =       1,
    IPSEC_STATE_POLICY_OK   =       2
};

#define IPSEC_STATE_OK (IPSEC_STATE_SA_OK | IPSEC_STATE_POLICY_OK)

struct ipsec_tunnel_s {
    xfrm_address_t saddr;
    xfrm_address_t daddr;
    uint32_t reqid;
};

#define MAX_KEY 4096
#define MAX_ALG_NAME 64

struct ipsec_algo {
	char		    alg_name[MAX_ALG_NAME];
	unsigned int	alg_key_len;
	unsigned int	alg_icv_len;
	char		    alg_key[MAX_KEY];
};

struct ipsec_sa_s {
    struct xfrm_usersa_info sa_info;
    struct xfrm_userpolicy_info policy_info;
    struct xfrm_encap_tmpl encap_info;
    struct ipsec_algo aead;
    struct ipsec_algo auth;
    struct ipsec_algo crypt;
    struct ipsec_algo comp;


    rte_atomic32_t refcnt;
    uint8_t used;
    uint32_t index;
    uint8_t del;

    uint8_t state; // 
};

int ipsecvpn_sa_init(void);


#endif
