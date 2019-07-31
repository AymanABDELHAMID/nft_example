/*
 * common.h
 *
 *  Created on: Jul 20, 2019
 *      Author: hasnaa
 */

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stddef.h>	/* for offsetof */
#include <errno.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <linux/netfilter.h>
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nf_tables.h>

#include <libmnl/libmnl.h>
#include <libnftnl/table.h>
#include <libnftnl/chain.h>
#include <libnftnl/rule.h>
#include <libnftnl/expr.h>
#include <libnftnl/obj.h>

#include <libnftnl/udata.h>


#include "../../Common/types.h"
#include "../../Common/helper_macros.h"
#include "../../Common/Logger/Logger.h"

#define UNKNOWN -1
#define NONE  	 0

#define SADDR "192.168.180.1"
#define DADDR "192.168.180.3"

#define HTTP_PORT   80
#define HTTPS_PORT  443
#define TCP_PORT 	445
#define SSH_PORT    22



#endif /* SRC_COMMON_H_ */
