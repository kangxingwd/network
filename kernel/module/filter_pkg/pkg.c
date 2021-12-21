/*在各个钩子上抓包
*author:zlf
*date:20091229
*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <net/icmp.h>                   /* for icmp_send */
#include <net/route.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/inet.h>
#include <linux/timer.h>
#define VTCP_OPT 0x13	/*vtcp code*/
#define VTCP_OLEN 0x04	/*vtcp len*/
enum 
{
	CAP_IP = 0,
	CAP_TCP,
	CAP_UDP,
	CAP_ICMP,
	CAP_LAST
};
enum
{
	TCP_SYN = (1 << 0),
	TCP_FIN = (1 << 1),
	TCP_RST = (1 << 2),
	TCP_PSH = (1 << 3),
	TCP_ACK = (1 << 16),
	TCP_LAST = (~0)
};
//监控特点的IP.id数据包
unsigned short sipid = 0;

/*这些变量用于参数传递，不加g_*/
unsigned short sport = 0;
unsigned short dport = 0;
unsigned short port = 0;
char* ip1 = NULL;
char* ip2 = NULL;
char* sip = NULL;
char* dip = NULL;
int tcp_type = TCP_LAST;/*1 syn, 2 fin, 4 rst, 8 psh, 16, ack*/
int vtcp_packet = 0;
int which_pkt = 0;
int proto = 0;/*0 ip, 1 tcp, 2 udp, 3 icmp, 4 other*/
int hook = 5;
int prio = NF_IP_PRI_FIRST;
int action = 0;/*0放行*/
unsigned int g_nip = 0;
unsigned int g_nip2 = 0;
unsigned int g_nsip = 0;
unsigned int g_ndip = 0;
#define START_STRACE 1
#define NOT_START 0
typedef struct tcp_packet_info
{
	unsigned int count;
	unsigned int ip;
	unsigned short port;
	unsigned short stat;
}tcp_packet_info;
struct tcp_packet_info g_one_conn = {0};
#define NF_IP_PRE_ROUTING	0
/* If the packet is destined for this box. */
#define NF_IP_LOCAL_IN		1
/* If the packet is destined for another interface. */
#define NF_IP_FORWARD		2
/* Packets coming from a local process. */
#define NF_IP_LOCAL_OUT		3
/* Packets about to hit the wire. */
#define NF_IP_POST_ROUTING	4
static const char *hook_name_tb[] =
{
	[NF_IP_PRE_ROUTING] = "pre_routing",
	[NF_IP_LOCAL_IN] = "local_in",
	[NF_IP_FORWARD] = "forward",
	[NF_IP_LOCAL_OUT] = "local_out",
	[NF_IP_POST_ROUTING] = "post_routing"
};
const char* ip_proto_name(int pkt_proto)
{
	switch(pkt_proto)
	{
		case IPPROTO_TCP:
		{
			return "tcp";
		}
		case IPPROTO_UDP:
		{
			return "udp";
		}
		case IPPROTO_ICMP:
		{
			return "icmp";
		}
		case IPPROTO_ESP:
		{
			return "esp";
		}
		case IPPROTO_AH:
		{
			return "ah";
		}		
		default:
		{
			return "unknown";
		}
	}
}
int is_proto_right(int pkt_proto)
{
	switch(proto)
	{
		case CAP_IP:/*ip*/
		{
			break;
		}
		case CAP_TCP:/*tcp*/
		{
			if(pkt_proto != IPPROTO_TCP)
			{
				return 0;
			}
			break;
		}
		case CAP_UDP:/*udp*/
		{
			if(pkt_proto != IPPROTO_UDP)
			{
				return 0;
			}		
			break;
		}
		case CAP_ICMP:/*icmp*/
		{
			if(pkt_proto != IPPROTO_ICMP)
			{
				return 0;
			}		
			break;
		}	
		default:
		{
			break;
		}
	}	
	return 1;
}

int is_ip_right(unsigned int pkt_sip, unsigned int pkt_dip)
{
	if(g_nsip && (g_nsip == pkt_sip))
	{
		return 1;
	}
	if(g_ndip && (g_ndip == pkt_dip))
	{
		return 1;
	}
	if(g_nip && ((g_nip == pkt_sip) || (g_nip == pkt_dip)))
	{
		return 1;
	}
	
	if(g_nip2 && ((g_nip2 == pkt_sip) || (g_nip2 == pkt_dip)))
	{
		return 1;
	}	
	return 0;
}
int is_port_right(unsigned short pkt_sport, unsigned short pkt_dport)
{
	if(sport && (sport != ntohs(pkt_sport)))
	{
		return 0;
	}
	if(dport && (dport != ntohs(pkt_dport)))
	{
		return 0;
	}
	if(port && (port != ntohs(pkt_sport)) && (port != ntohs(pkt_dport)))
	{
		return 0;
	}
	return 1;
	
}
int is_vtcp_syn(struct sk_buff *skb)
{
	struct iphdr *iph = NULL;
	struct tcphdr *tcph = NULL;
	unsigned char *ptr = NULL;
	int iphl = 0, len = 0, opcode = 0, opsize = 0;
	iph = (struct iphdr *)skb_network_header(skb);
	iphl = (iph->ihl << 2);
	tcph = (void *)iph + iphl;
	ptr = (unsigned char*)(tcph + 1);
	len = (tcph->doff*4) - sizeof(struct tcphdr);
	while(len > 0)
	{
		opcode = *ptr++;
		switch(opcode)
		{
			case 0: /*end*/
			{
				return 0;
			}
			case 1:/*non op Ref: RFC 793 section 3.1 */
			{
				len--;
				continue;
			}
			default:
			{
				opsize = *ptr++;
				if(opsize < 2)/* "silly options" */
				{
					return 0;
				}
				if(opsize > len)
				{
					return 0;/* don't parse partial options */
				}
				switch(opcode)
				{
					case VTCP_OPT:
					{
						if(opsize == VTCP_OLEN)
						{
							return 1;
						}
						break;
					}
					default:
					{
						break;
					}
				}
				ptr += opsize -2;
				len -= opsize;
			}
		}
	}	
	return 0;
}

int is_tcp_type_right(unsigned int hooknum, struct sk_buff *skb, char* buf, int len)
{
	struct iphdr *iph = NULL;
	struct tcphdr *tcph = NULL;
	int pos = 0;
	int iphl = 0;
	iph = (struct iphdr *)skb_network_header(skb);
	iphl = (iph->ihl << 2);
	tcph = (void *)iph + iphl;
	if(tcp_type <= 0 ||tcp_type > 31)
	{
		tcp_type = TCP_LAST;
	}
	if(hooknum == NF_IP_LOCAL_IN || hooknum == NF_IP_LOCAL_OUT) 
	{
		if(g_one_conn.stat == START_STRACE)
		{
			if(((iph->saddr == g_one_conn.ip) && (tcph->source == g_one_conn.port))||
				((iph->daddr == g_one_conn.ip) && (tcph->dest == g_one_conn.port)))
				
			{
				//pr_info("packet %d\n", g_one_conn.count);
				g_one_conn.count ++;
			}
		}
	}
	if(tcp_type&TCP_SYN)
	{
		if(tcph->syn)
		{
			if(vtcp_packet == 1)
			{
				if(!is_vtcp_syn(skb))
				{
					return 0;
				}
				else
				{
					pos += snprintf(buf + pos, len - pos, " vsyn");
				}
				
			}
			else
			{
				pos += snprintf(buf + pos, len - pos, " syn");
			}
			if(hooknum == NF_IP_LOCAL_IN || hooknum == NF_IP_LOCAL_OUT) 
			{
				if(g_one_conn.stat != START_STRACE)
				{
					pr_info("start trace %u.%u.%u.%u:%d %u.%u.%u.%u:%d\n", 
						NIPQUAD(iph->saddr), ntohs(tcph->source), NIPQUAD(iph->daddr), ntohs(tcph->dest));
					g_one_conn.count ++;
					g_one_conn.ip = iph->saddr;
					g_one_conn.port = tcph->source;
					g_one_conn.stat = START_STRACE;
				}
			}
		}
		else
		{
			if(tcp_type != TCP_LAST)
			{
				return 0;
			}
		}
	}
	if(tcp_type&TCP_FIN)
	{
		if(tcph->fin)
		{
			pos += snprintf(buf + pos, len - pos, " fin");
		}
		else
		{
			if(tcp_type != TCP_LAST)
			{
				return 0;
			}
		}
	}
	if(tcp_type&TCP_RST)
	{
		if(tcph->rst)
		{
			pos += snprintf(buf + pos, len - pos, " rst");
		}
		else
		{
			if(tcp_type != TCP_LAST)
			{
				return 0;
			}
		}
	}
	if(tcp_type&TCP_PSH)
	{
		if(tcph->psh)
		{
			pos += snprintf(buf + pos, len - pos, " psh");
		}
		else
		{
			if(tcp_type != TCP_LAST)
			{
				return 0;
			}
		}
	}
	if(tcp_type&TCP_ACK)
	{
		if(tcph->ack)
		{
			pos += snprintf(buf + pos, len - pos, " ack");
		}
		else
		{
			if(tcp_type != TCP_LAST)
			{
				return 0;
			}
		}
	}
	if(tcph->urg)
	{
		pos += snprintf(buf + pos, len - pos, " urg");
	}
	if(tcph->ece)
	{
		pos += snprintf(buf + pos, len - pos, " ece");
	}
	if(tcph->cwr)
	{
		pos += snprintf(buf + pos, len - pos, " cwr");
	}
	return 1;
}

static unsigned int pinfo_hook(unsigned int hooknum,
			       struct sk_buff *skb,
			       const struct net_device *in,
			       const struct net_device *out,
			       int (*okfn)(struct sk_buff *))
{

	struct iphdr *iph = NULL;
	struct tcphdr *tcph = NULL;
	struct udphdr *udph = NULL;
	int ret = 0;
	char buf[128] = {0};
	iph = (struct iphdr *)skb_network_header(skb);
	tcph = (struct tcphdr *)(skb->data + (iph->ihl << 2));
	udph =  (struct udphdr *)(skb->data + (iph->ihl << 2));
	if(!is_proto_right(iph->protocol))
	{
		return NF_ACCEPT;
	}
	if(!is_ip_right(iph->saddr, iph->daddr))
	{
		return NF_ACCEPT;
	}
	if(iph->protocol == IPPROTO_TCP)
	{	
		if(!is_port_right(tcph->source, tcph->dest))
		{
			return NF_ACCEPT;
		}
		ret = is_tcp_type_right(hooknum, skb, buf, sizeof(buf));
		if(hooknum == NF_IP_LOCAL_IN || hooknum == NF_IP_LOCAL_OUT) 
		{
			if(g_one_conn.count == which_pkt)
			{
				//pr_info("trace hit %d\n", g_one_conn.count);
				memset(&g_one_conn, 0, sizeof(g_one_conn));
				pr_info("the tcp %d packet %s, drop proto %s %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u info:%s\n", 
					which_pkt, hook_name_tb[hooknum], ip_proto_name(iph->protocol), NIPQUAD(iph->saddr), ntohs(tcph->source), NIPQUAD(iph->daddr), ntohs(tcph->dest), buf);
				return NF_DROP;
			}
		}
		if(!ret)
		{

			return NF_ACCEPT;
		}
	}
	else if(iph->protocol == IPPROTO_UDP)
	{
		if(!is_port_right(udph->source, udph->dest))
		{
			return NF_ACCEPT;
		}
	}
	else if(iph->protocol == IPPROTO_ICMP)
	{
		
	}
	
	if(action)
	{
		pr_info("%s, id=%x time %d HZ %d drop proto %s %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u info:%s\n", 
			hook_name_tb[hooknum], iph->id, (int)jiffies, (int)HZ, ip_proto_name(iph->protocol), NIPQUAD(iph->saddr), ntohs(tcph->source), NIPQUAD(iph->daddr), ntohs(tcph->dest), buf);
		return NF_DROP;
	}
	else if(hooknum >= 0 && hooknum <= NF_IP_POST_ROUTING)
	{
		unsigned short tmpipid = ntohs(iph->id);
		if (sipid && sipid == tmpipid)
		{
			dump_stack();
		}
			
		if (iph->protocol == IPPROTO_UDP)
		{
			short dateid = 0;
			//memcpy((void*)&dateid,(void*)&skb->data[26],2); //data从IP头开始
			memcpy((void*)&dateid,(void*)&skb->data[54],2);
			pr_info("%s, id=%2x,dataid=%2x time %d HZ %d proto %s %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u info:%s\n", 
				hook_name_tb[hooknum], ntohs(iph->id), ntohs(dateid), (int)jiffies, (int)HZ, ip_proto_name(iph->protocol), NIPQUAD(iph->saddr), ntohs(udph->source), NIPQUAD(iph->daddr), ntohs(udph->dest), buf);			
		}
		else
		{
			pr_info("%s, id=%2x time %d HZ %d proto %s %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u info:%s\n", 
							hook_name_tb[hooknum], ntohs(iph->id), (int)jiffies, (int)HZ, ip_proto_name(iph->protocol), NIPQUAD(iph->saddr), ntohs(tcph->source), NIPQUAD(iph->daddr), ntohs(tcph->dest), buf);			
		}

	}
	return NF_ACCEPT;
}
#define MAX_HOOK_NUM 128
struct capture_hook
{
	int hooked;/*是否已经加载*/
	struct nf_hook_ops hook;/*钩子*/
};
enum 
{
	HOOK_PRE_ROUTING = 0, /*pre_routing*/
	HOOK_LOCAL_IN,/*local_in */	
	HOOK_FORWARD, /*forward*/	
	HOOK_LOCAL_OUT, /*local_out*/
	HOOK_POST_ROUTING, /*post_routing*/
	HOOK_LAST/*last*/
};
static struct capture_hook g_capture_hook_array[HOOK_LAST] = 
{
	[HOOK_PRE_ROUTING] = {/*pre_routing first*/
			.hooked = 0,
			.hook = {
					.hook = pinfo_hook,
					.pf = PF_INET,
					.hooknum = NF_IP_PRE_ROUTING,
					.priority = NF_IP_PRI_FIRST
			}

	},
	[HOOK_LOCAL_IN] = {/*local_in first*/
			.hooked = 0,
			.hook = {
					.hook = pinfo_hook,
					.pf = PF_INET,
					.hooknum = NF_IP_LOCAL_IN,
					.priority = NF_IP_PRI_FIRST
			}

	},
	[HOOK_FORWARD] = {/*forward first*/
			.hooked = 0,
			.hook = {
					.hook = pinfo_hook,
					.pf = PF_INET,
					.hooknum = NF_IP_FORWARD,
					.priority = NF_IP_PRI_FIRST
			}

	},
	[HOOK_LOCAL_OUT] = {/*local_out first*/
			.hooked = 0,
			.hook = {
					.hook = pinfo_hook,
					.pf = PF_INET,
					.hooknum = NF_IP_LOCAL_OUT,
					.priority = NF_IP_PRI_FIRST
			}

	},
	[HOOK_POST_ROUTING] = {/*post_routing first*/
			.hooked = 0,
			.hook = {
					.hook = pinfo_hook,
					.pf = PF_INET,
					.hooknum = NF_IP_POST_ROUTING,
					.priority = NF_IP_PRI_FIRST
			}

	}
};

/*
* @func:初始化模块
* @ret:0成功，< 0失败
*/
static int pkttracer_init(void)
{
	int ret, i;
	if(ip1)
	{
		g_nip = in_aton(ip1);
		pr_info("pkttracer_init ip1 = %s \n", ip1);
	}
	if(ip2)
	{
		g_nip2 = in_aton(ip2);
		pr_info("pkttracer_init ip2 = %s \n", ip2);
	}	
	if(sip)
	{
		g_nsip = in_aton(sip);
	}
	if(dip)
	{
		g_ndip = in_aton(dip);
	}

	pr_info("pkttracer_init sport = %d, dport = %d, port=%d \n", sport,dport,port);
	
	pr_info("pkttracer_init sipid = %d\n", sipid);
	
	for(i = 0; i < HOOK_LAST; ++i)
	{
		if(hook == i || hook == 5)
		{
			g_capture_hook_array[i].hook.priority = prio;
			if((ret = nf_register_hook(&g_capture_hook_array[i].hook)) < 0)
			{
				pr_info("hook %d failed ret %d\n", i, ret);
				g_capture_hook_array[i].hooked = 0;
			}
			else
			{
				g_capture_hook_array[i].hooked = 1;
			}
		}
	}
	return 0;
}
/*
* @func:卸载模块
* @ret:无
*/
static void pkttracer_cleanup(void)
{
	int i;
	for(i = 0; i < HOOK_LAST; ++i)
	{
		if(g_capture_hook_array[i].hooked)
		{
			nf_unregister_hook(&g_capture_hook_array[i].hook);
			g_capture_hook_array[i].hooked = 0;
		}
	}
}
/*
* @func:模块入口
*/
module_init(pkttracer_init);
/*
* @func:模块出口
*/
module_exit(pkttracer_cleanup);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("a packet tracer");
MODULE_AUTHOR("zlf@sinfor");
module_param(hook, int, 0);
MODULE_PARM_DESC(hook, "0 pre_routing, 1 local_in, 2 forward, 3 local_out, 4 post_routing,5 all");
module_param(prio, int, 0);
MODULE_PARM_DESC(prio, "hook priority");
module_param(proto, int, 0);
MODULE_PARM_DESC(proto, "0 ip, 1 tcp, 2 udp, 3 icmp, 4 other");
module_param(ip1, charp, 0);
MODULE_PARM_DESC(ip1, "source or dest ip address");
module_param(ip2, charp, 0);
MODULE_PARM_DESC(ip2, "source or dest ip address");
module_param(sip, charp, 0);
MODULE_PARM_DESC(sip, "source ip address");
module_param(dip, charp, 0);
MODULE_PARM_DESC(dip, "dest ip address");
module_param(port, short, 0);
MODULE_PARM_DESC(port, "source or dest port");
module_param(sport, short, 0);
MODULE_PARM_DESC(sport, "source port");
module_param(dport, short, 0);
MODULE_PARM_DESC(dport, "dest port");
module_param(action, int, 0);
MODULE_PARM_DESC(action, "0 accept, other drop");
module_param(tcp_type, int, 0);
MODULE_PARM_DESC(tcp_type, "1 syn, 2 fin, 4 rst, 8 psh, 16, ack (0~31 is valid)");
module_param(vtcp_packet, int, 0);
MODULE_PARM_DESC(vtcp_packet, "1 vtcp syn packet, other normal tcp");
module_param(which_pkt, int, 0);
MODULE_PARM_DESC(which_pkt, "which pkt in tcp");
module_param(sipid, short, 0);
MODULE_PARM_DESC(sipid, "ip.id");


			