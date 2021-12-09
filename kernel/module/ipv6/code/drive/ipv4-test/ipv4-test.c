#include<linux/init.h>  //初始换函数
#include<linux/kernel.h>  //内核头文件
#include<linux/module.h>  //模块的头文件
#include<linux/netfilter.h>
#include<linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <arpe/inet.h>

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

static unsigned int ipv4_test_pre_routing(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
    printk("ipv4_test_pre_routing \n");

	return NF_ACCEPT;
}

static unsigned int ipv4_test_forward(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
    printk("ipv4_test_forward \n");
	return NF_ACCEPT;
}

// net\bridge\br_multicast.c  br_ip4_multicast_alloc_query      IPPROTO_IGMP

static unsigned int ipv4_test_local_in(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
    struct iphdr *iph;
    // struct tcphdr *tcph;
    // printk("ipv4_test_local_in \n");
    
    if (unlikely(!skb)) {
        return NF_ACCEPT;
    }
    
    iph = ip_hdr(skb);
    if (unlikely(!iph)) {
        return NF_ACCEPT;
    }

    if (iph->protocol == IPPROTO_ICMP) {
        printk("[ipv4_test] icmp \n");
        IPH_PRINT_IP(iph);

    } else if (iph->protocol == IPPROTO_UDP) {
        // printk("[ipv4_test] udp \n");
        

    } else if (iph->protocol == IPPROTO_TCP) {
        // printk("[ipv4_test] tcp \n");

    } else {
        // printk("[ipv4_test], iph protocol =  %u\n", iph->protocol);
    }

	return NF_ACCEPT;
}

static unsigned int ipv4_test_local_out(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
    printk("ipv4_test_local_out \n");
	return NF_ACCEPT;
}

static unsigned int ipv4_test_post_routing(void *priv,
				      struct sk_buff *skb,
				      const struct nf_hook_state *state)
{
    printk("ipv4_test_post_routing \n");
	return NF_ACCEPT;
}

static const struct nf_hook_ops ipv4_test_conntrack_ops[] = {
	// {
	// 	.hook		= ipv4_test_pre_routing,
	// 	.pf		= NFPROTO_IPV4,
	// 	.hooknum	= NF_INET_PRE_ROUTING,
	// 	.priority	= NF_IP_PRI_FILTER + 1,
	// },
    // {
	// 	.hook		= ipv4_test_forward,
	// 	.pf		= NFPROTO_IPV4,
	// 	.hooknum	= NF_INET_FORWARD,
	// 	.priority	= NF_IP_PRI_FILTER + 1,
	// },
	{
		.hook		= ipv4_test_local_in,
		.pf		= NFPROTO_IPV4,
		.hooknum	= NF_INET_LOCAL_IN,
		.priority	= NF_IP_PRI_FILTER + 1,
	},
    // {
	// 	.hook		= ipv4_test_local_out,
	// 	.pf		= NFPROTO_IPV4,
	// 	.hooknum	= NF_INET_LOCAL_OUT,
	// 	.priority	= NF_IP_PRI_FILTER + 1,
	// },
    // {
	// 	.hook		= ipv4_test_post_routing,
	// 	.pf		= NFPROTO_IPV4,
	// 	.hooknum	= NF_INET_POST_ROUTING,
	// 	.priority	= NF_IP_PRI_FILTER + 1,
	// },
};

static int ipv4_test_init(void) 
{
    int err = 0;
    printk("ipv4_test_init!\n");

    err = nf_register_net_hooks(&init_net, ipv4_test_conntrack_ops, ARRAY_SIZE(ipv4_test_conntrack_ops));  

    printk("ipv6_test_init ok!\n");
    return 0; 
}

static void ipv4_test_exit(void) 
{ 
    nf_unregister_net_hooks(&init_net, ipv4_test_conntrack_ops, ARRAY_SIZE(ipv4_test_conntrack_ops)); 
    printk("ipv4_test_exit!\n");
} 

module_init(ipv4_test_init);
module_exit(ipv4_test_exit); 
MODULE_LICENSE("Dual BSD/GPL");
