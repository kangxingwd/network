/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

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

#include "log.h"
#include "ipsecvpn_sa.h"


static int lcore_hello(__rte_unused void *arg)
{
	unsigned lcore_id;
	lcore_id = rte_lcore_id();
	printf("hello from core %u\n", lcore_id);

	while(1) {
		sleep(1000);
	}

	return 0;
}

int
main(int argc, char **argv)
{
	int ret;
	unsigned lcore_id;

	LOG_INIT(LOG_MODE_STDOUT, LOG_LEVEL_INFO, NULL);

	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_panic("Cannot init EAL\n");

	ret = ipsecvpn_sa_init();
	if (ret != 0) {
		LOG_DEBUG_S("ipsecvpn sa init failed\n");
		return 1;
	}

	/* call lcore_hello() on every slave lcore */
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		rte_eal_remote_launch(lcore_hello, NULL, lcore_id);
	}

	printf("main---\n");
	/* call it on master lcore too */
	lcore_hello(NULL);

	rte_eal_mp_wait_lcore();
	return 0;
}
