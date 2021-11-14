#include<linux/init.h>  //初始换函数
#include<linux/kernel.h>  //内核头文件
#include<linux/module.h>  //模块的头文件
#include<linux/netfilter.h>
#include<linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/in6.h>
#include <net/ipv6.h>
#include <linux/icmpv6.h>

#include "myip-utils.h"

// net\netfilter\core.c
// include\linux\netfilter.h        nf_register_net_hook  nf_unregister_net_hookss
/*
// include\uapi\linux\netfilter.h
#define NF_DROP 0
#define NF_ACCEPT 1
#define NF_STOLEN 2
#define NF_QUEUE 3
#define NF_REPEAT 4
#define NF_STOP 5	Deprecated, for userspace nf_queue compatibility.
#define NF_MAX_VERDICT NF_STOP
*/

/*
// nf_register_hook在新版内核里面换成了 nf_register_net_hook(struct net *net, const struct nf_hook_ops *ops);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
nf_register_net_hook(&init_net, &reg)  //&init_net 可直接使用
#else
nf_register_hook(&reg)
#endif
*/

/*
// 自从kernel4.13开始 hook函数的原型就是
int sample_nf_hookfn(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);
*/

static unsigned int ipv6_test_pre_routing(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
    printk("ipv6_test_pre_routing \n");

	return NF_ACCEPT;
}

static unsigned int ipv6_test_forward(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
    printk("ipv6_test_forward \n");
	return NF_ACCEPT;
}

// net\bridge\br_multicast.c  br_ip4_multicast_alloc_query      IPPROTO_IGMP

static unsigned int ipv6_test_local_in(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
	struct ipv6hdr *ip6h = ipv6_hdr(skb);
	unsigned char pnum = ip6h->nexthdr;
	int protoff;
	__be16 frag_off;
    // struct tcphdr *tcph;
    printk("ipv6_test_local_in \n");

	// skb_network_offset(skb)
	printk("ip6h->version: %u\n", ip6h->version);
	printk("ip6h->priority: %u\n", ip6h->priority);
	printk("ip6h->flow_lbl: 0x%02x%02x%02x\n", ip6h->flow_lbl[0],ip6h->flow_lbl[1], ip6h->flow_lbl[2]);
	printk("ip6h->payload_len: %u\n", ntohs(ip6h->payload_len));
	printk("ip6h->nexthdr: %u\n", ip6h->nexthdr);
	printk("ip6h->hop_limit: %u\n", ip6h->hop_limit);
	IP6_PRINT_IP(ip6h);

	protoff = ipv6_skip_exthdr(skb, sizeof(struct ipv6hdr), &pnum, &frag_off);
	if (protoff < 0 || (frag_off & htons(~0x7)) != 0) {
		pr_debug("proto header not found\n");
		goto out;
	}

	if (pnum == IPPROTO_ICMPV6) {
		printk("[ipv6_test] icmp \n");
		const struct icmp6hdr *hp;
		struct icmp6hdr _hdr;

		hp = skb_header_pointer(skb, protoff, sizeof(_hdr), &_hdr);
		if (hp == NULL)
			goto out;
		
		printk("icmp6_type: %u\n", hp->icmp6_type);
		printk("icmp6_identifier: %04x\n", ntohs(hp->icmp6_identifier));
		printk("icmp6_code: %u\n", hp->icmp6_code);
		printk("icmp6_sequence: 0x%04x, %u\n", ntohs(hp->icmp6_sequence), ntohs(hp->icmp6_sequence));
    } else {
        printk("[ipv6_test], ip6h nexthdr =  %u\n", pnum);
    }

	// ipv6_pkt_to_tuple

	// tcph = (void *)skb->data + protoff;

out:
	return NF_ACCEPT;
}

static unsigned int ipv6_test_local_out(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
    printk("ipv6_test_local_out \n");
	return NF_ACCEPT;
}

static unsigned int ipv6_test_post_routing(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
    printk("ipv6_test_post_routing \n");
	return NF_ACCEPT;
}

static const struct nf_hook_ops ipv6_test_conntrack_ops[] = {
	// {
	// 	.hook		= ipv6_test_pre_routing,
	// 	.pf		= NFPROTO_IPV6,
	// 	.hooknum	= NF_INET_PRE_ROUTING,
	// 	.priority	= NF_IP_PRI_FILTER + 1,
	// },
    // {
	// 	.hook		= ipv6_test_forward,
	// 	.pf		= NFPROTO_IPV6,
	// 	.hooknum	= NF_INET_FORWARD,
	// 	.priority	= NF_IP_PRI_FILTER + 1,
	// },
	{
		.hook		= ipv6_test_local_in,
		.pf		= NFPROTO_IPV6,
		.hooknum	= NF_INET_LOCAL_IN,
		.priority	= NF_IP_PRI_FILTER + 1,
	},
    // {
	// 	.hook		= ipv6_test_local_out,
	// 	.pf		= NFPROTO_IPV6,
	// 	.hooknum	= NF_INET_LOCAL_OUT,
	// 	.priority	= NF_IP_PRI_FILTER + 1,
	// },
    // {
	// 	.hook		= ipv6_test_post_routing,
	// 	.pf		= NFPROTO_IPV6,
	// 	.hooknum	= NF_INET_POST_ROUTING,
	// 	.priority	= NF_IP_PRI_FILTER + 1,
	// },
};

static int ipv6_test_init(void) 
{
    int err = 0;
    printk("ipv6_test_init!\n");

    err = nf_register_net_hooks(&init_net, ipv6_test_conntrack_ops, ARRAY_SIZE(ipv6_test_conntrack_ops));  

    printk("ipv6_test_init ok!\n");
    return 0; 
}

static void ipv6_test_exit(void) 
{ 
    nf_unregister_net_hooks(&init_net, ipv6_test_conntrack_ops, ARRAY_SIZE(ipv6_test_conntrack_ops)); 
    printk("ipv6_test_exit!\n");
} 

module_init(ipv6_test_init);
module_exit(ipv6_test_exit); 
MODULE_LICENSE("Dual BSD/GPL");
