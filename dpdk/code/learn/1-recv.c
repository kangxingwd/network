#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/queue.h>
#include <unistd.h>

#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>

#include <inttypes.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_mbuf.h>

#define NUM_MBUF (4096-1)

/* How many packets to attempt to read from NIC in one go */
#define PKT_BURST_SZ            32

/* How many objects (mbufs) to keep in per-lcore mempool cache */
#define MEMPOOL_CACHE_SZ        PKT_BURST_SZ

static struct rte_mempool *mbuf_pool = NULL;

static struct rte_eth_conf default_port_conf = {
	.rxmode = {
		.split_hdr_size = 0,
	},
	.txmode = {
		.mq_mode = ETH_MQ_TX_NONE,
	},
};

// static const struct rte_eth_conf port_conf_default = {
// 	.rxmode = {
// 		.max_rx_pkt_len = RTE_ETHER_MAX_LEN,
// 	},
// };

#define RTE_TEST_RX_DESC_DEFAULT 1024
#define RTE_TEST_TX_DESC_DEFAULT 1024
/* Static global variables used within this file. */
static uint16_t nb_rxd = RTE_TEST_RX_DESC_DEFAULT;
static uint16_t nb_txd = RTE_TEST_TX_DESC_DEFAULT;

/* ethernet addresses of ports */
static struct rte_ether_addr test_ports_eth_addr[RTE_MAX_ETHPORTS];

static void ng_port_init(void)
{
    uint16_t portid;
    uint16_t nb_ports; 
    
    nb_ports = rte_eth_dev_count_avail();
	if (nb_ports == 0) {
        rte_exit(EXIT_FAILURE, "No Ethernet ports - bye\n");
    }
	
    uint8_t nb_rx_queue;

    RTE_ETH_FOREACH_DEV(portid) {
        struct rte_eth_dev_info dev_info;
        struct rte_eth_rxconf rxq_conf;
        struct rte_eth_txconf txq_conf;
        int ret;

		printf("Initializing port %u... \n", portid);
        fflush(stdout);
        rte_eth_dev_info_get(portid, &dev_info);

        printf("portid: %u ,driver_name: %s, max_rx_queues: %u, max_tx_queues: %u, nb_rx_queues: %u, nb_tx_queues: %u\n ",
            portid,
            dev_info.driver_name, 
            dev_info.max_rx_queues,
            dev_info.max_tx_queues, 
            dev_info.nb_rx_queues, 
            dev_info.nb_tx_queues
            );

        struct rte_eth_conf port_conf =  default_port_conf;
        ret = rte_eth_dev_configure(portid, 1, 1, &port_conf);
		if (ret < 0) {
            rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d, port=%u\n", ret, portid);
        }

        rte_eth_macaddr_get(portid, &test_ports_eth_addr[portid]);

        rxq_conf = dev_info.default_rxconf;
	    rxq_conf.offloads = port_conf.rxmode.offloads;
        ret = rte_eth_rx_queue_setup(portid, 0, nb_rxd, rte_eth_dev_socket_id(portid), &rxq_conf, mbuf_pool);
		if (ret < 0) {
            rte_exit(EXIT_FAILURE, "rte_eth_rx_queue_setup:err=%d, port=%u\n", ret, portid);
        }

        txq_conf = dev_info.default_txconf;
	    txq_conf.offloads = port_conf.txmode.offloads;
	    ret = rte_eth_tx_queue_setup(portid, 0, nb_txd, rte_eth_dev_socket_id(portid), &txq_conf);
	    if (ret < 0) {
            rte_exit(EXIT_FAILURE, "Could not setup up TX queue for port%u (%d)\n", (unsigned)portid, ret);
        }
		
        ret = rte_eth_dev_start(portid);
        if (ret < 0) {
            rte_exit(EXIT_FAILURE, "Could not start port%u (%d)\n", (unsigned)portid, ret);
        }

        // rte_eth_promiscuous_enable(portid);

        printf("Port %u, MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n\n",
				portid,
				test_ports_eth_addr[portid].addr_bytes[0],
				test_ports_eth_addr[portid].addr_bytes[1],
				test_ports_eth_addr[portid].addr_bytes[2],
				test_ports_eth_addr[portid].addr_bytes[3],
				test_ports_eth_addr[portid].addr_bytes[4],
				test_ports_eth_addr[portid].addr_bytes[5]);
        
        /* Enable RX in promiscuous mode for the Ethernet device. */
        // 5. 混杂模式收包
        rte_eth_promiscuous_enable(portid);
	}

    printf("port init ok!\n");
}

static int
main_loop(__attribute__((unused)) void *arg)
{
	unsigned lcore_id;
	lcore_id = rte_lcore_id();
	printf("hello from core %u\n", lcore_id);
	return 0;
}

int main(int argc, char *argv[])
{
    unsigned lcore_id;
    uint16_t portid;
    int ret = rte_eal_init(argc, argv);
	if (ret < 0)
        rte_exit(EXIT_FAILURE, "Could not initialise EAL (%d)\n", ret);

    mbuf_pool = rte_pktmbuf_pool_create("mbuf_pool", NUM_MBUF, MEMPOOL_CACHE_SZ, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
	if (mbuf_pool == NULL) {
		rte_exit(EXIT_FAILURE, "Could not initialise mbuf pool\n");
		return -1;
	}

    ng_port_init();

    // while (1) {

    //     struct rte_mbuf *pkts_burst[BURST_SIZE];
    //     int nb_rx = rte_eth_rx_burst(0, 0, pkts_burst, MAX_PKT_BURST);

    //     int j = 0;
    //     for (j = 0; j < nb_rx; j++) {

	// 	}


    // }

    // rte_eal_mp_remote_launch(main_loop, NULL, CALL_MASTER);
	// RTE_LCORE_FOREACH_SLAVE(lcore_id) {
	// 	if (rte_eal_wait_lcore(lcore_id) < 0)
	// 		return -1;
	// }

    /* call lcore_hello() on every slave lcore */
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		rte_eal_remote_launch(main_loop, NULL, lcore_id);
	}

    printf("master...\n");

    rte_eal_mp_wait_lcore();

    /* stop ports */
	RTE_ETH_FOREACH_DEV(portid) {
		printf("Closing port %d...", portid);
		rte_eth_dev_stop(portid);
		rte_eth_dev_close(portid);
		printf(" Done\n");
	}
	printf("Bye...\n");

    return 0;
}

