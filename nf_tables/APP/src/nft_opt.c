/*
 * libnftnl.c
 *
 *  Created on: Jul 15, 2019
 *      Author: hasnaa
 */

#include "nft_opt.h"


//--------------------------------------------------------------------
struct nftnl_table *nft_add_table_impl(char *tbl, uint32_t family)
{
	struct nftnl_table *table;
	struct mnl_socket *nl;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	uint32_t portid, seq, table_seq;
	struct mnl_nlmsg_batch *batch;
	int ret;

	table = nftnl_table_alloc();
	if (table == NULL) {
		perror("OOM");
		return NULL;
	}

	nftnl_table_set_u32(table, NFTNL_TABLE_FAMILY, family);
	nftnl_table_set_str(table, NFTNL_TABLE_NAME, tbl);

	seq = time(NULL);
	batch = mnl_nlmsg_batch_start(buf, sizeof(buf));

	nftnl_batch_begin(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);

	table_seq = seq;
	family = nftnl_table_get_u32(table, NFTNL_TABLE_FAMILY);
	nlh = nftnl_table_nlmsg_build_hdr(mnl_nlmsg_batch_current(batch),
					NFT_MSG_NEWTABLE, family,
					NLM_F_CREATE|NLM_F_ACK, seq++);
	nftnl_table_nlmsg_build_payload(nlh, table);
	nftnl_table_free(table);
	mnl_nlmsg_batch_next(batch);

	nftnl_batch_end(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);

	nl = mnl_socket_open(NETLINK_NETFILTER);
	if (nl == NULL) {
		perror("mnl_socket_open");
		exit(EXIT_FAILURE);
	}

	if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
		perror("mnl_socket_bind");
		exit(EXIT_FAILURE);
	}
	portid = mnl_socket_get_portid(nl);

	if (mnl_socket_sendto(nl, mnl_nlmsg_batch_head(batch),
				  mnl_nlmsg_batch_size(batch)) < 0) {
		perror("mnl_socket_send");
		exit(EXIT_FAILURE);
	}

	mnl_nlmsg_batch_stop(batch);

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, table_seq, portid, NULL, NULL);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}
	if (ret == -1) {
		perror("error");
		exit(EXIT_FAILURE);
	}
	mnl_socket_close(nl);

return table;
}

//-----------------------------------------------------------------------
struct nftnl_chain *nft_add_chain_impl(int argc, char *argv[], uint32_t family, bool_t bchn)
{
	struct mnl_socket *nl;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	uint32_t portid, seq, chain_seq;
	struct nftnl_chain *chain;
	struct mnl_nlmsg_batch *batch;
	int hooknum = 0, ret;

	if ( argc == 6 || bchn ) {
		/* This is a base chain, set the hook number */
		if (strcmp(argv[4], "input") == 0)
			hooknum = NF_INET_LOCAL_IN;
		else if (strcmp(argv[4], "output") == 0)
			hooknum = NF_INET_LOCAL_OUT;
		else if (strcmp(argv[4], "prerouting") == 0)
			hooknum = NF_INET_PRE_ROUTING;
		else if (strcmp(argv[4], "postrouting") == 0)
			hooknum = NF_INET_POST_ROUTING;
		else if (strcmp(argv[4], "forward") == 0)
			hooknum = NF_INET_FORWARD;
		else {
			fprintf(stderr, "Unknown hook: %s\n", argv[4]);
			return NULL;
		}
	}

	chain = nftnl_chain_alloc();
	if (chain == NULL) {
		perror("OOM");
		return NULL;
	}
	nftnl_chain_set(chain, NFTNL_CHAIN_TABLE, argv[2]);
	nftnl_chain_set(chain, NFTNL_CHAIN_NAME, argv[3]);
	if ( argc == 6 || bchn ) {
		nftnl_chain_set_u32(chain, NFTNL_CHAIN_HOOKNUM, hooknum);
		nftnl_chain_set_u32(chain, NFTNL_CHAIN_PRIO, atoi(argv[5]));
	}

	seq = time(NULL);
	batch = mnl_nlmsg_batch_start(buf, sizeof(buf));

	nftnl_batch_begin(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);

	chain_seq = seq;
	nlh = nftnl_chain_nlmsg_build_hdr(mnl_nlmsg_batch_current(batch),
					NFT_MSG_NEWCHAIN, family,
					NLM_F_CREATE|NLM_F_ACK, seq++);
	nftnl_chain_nlmsg_build_payload(nlh, chain);
	nftnl_chain_free(chain);
	mnl_nlmsg_batch_next(batch);

	nftnl_batch_end(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);

	nl = mnl_socket_open(NETLINK_NETFILTER);
	if (nl == NULL) {
		perror("mnl_socket_open");
		exit(EXIT_FAILURE);
	}

	if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
		perror("mnl_socket_bind");
		exit(EXIT_FAILURE);
	}
	portid = mnl_socket_get_portid(nl);

	if (mnl_socket_sendto(nl, mnl_nlmsg_batch_head(batch),
			      mnl_nlmsg_batch_size(batch)) < 0) {
		perror("mnl_socket_send");
		exit(EXIT_FAILURE);
	}

	mnl_nlmsg_batch_stop(batch);

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, chain_seq, portid, NULL, NULL);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}
	if (ret == -1) {
		perror("error");
		exit(EXIT_FAILURE);
	}
	mnl_socket_close(nl);

	return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------
static void nft_add_payload(struct nftnl_rule *rule, uint32_t base, uint32_t dreg, uint32_t offset, uint32_t len)
{
	struct nftnl_expr *expr;

	expr = nftnl_expr_alloc("payload");
	if (expr == NULL) {
		perror("expr payload oom");
		exit(EXIT_FAILURE);
	}

	nftnl_expr_set_u32(expr, NFTNL_EXPR_PAYLOAD_BASE, base);
	nftnl_expr_set_u32(expr, NFTNL_EXPR_PAYLOAD_DREG, dreg);
	nftnl_expr_set_u32(expr, NFTNL_EXPR_PAYLOAD_OFFSET, offset);
	nftnl_expr_set_u32(expr, NFTNL_EXPR_PAYLOAD_LEN, len);

	nftnl_rule_add_expr(rule, expr);
}

//-------------------------------------------------------------------------
static void nft_add_cmp(struct nftnl_rule *rule, uint32_t sreg, uint32_t op,
		    const void *data, uint32_t data_len)
{
	struct nftnl_expr *e;

	e = nftnl_expr_alloc("cmp");
	if (e == NULL) {
		perror("expr cmp oom");
		exit(EXIT_FAILURE);
	}

	nftnl_expr_set_u32(e, NFTNL_EXPR_CMP_SREG, sreg);
	nftnl_expr_set_u32(e, NFTNL_EXPR_CMP_OP, op);
	nftnl_expr_set(e, NFTNL_EXPR_CMP_DATA, data, data_len);

	nftnl_rule_add_expr(rule, e);

}

//----------------------------------------------------------------------------
void nft_add_verdict(struct nftnl_rule *rule, uint32_t dreg,
	 const void *data, uint32_t data_len, const char *chain, Int8 verdict)
{
	struct nftnl_expr *ex_ver;

	ex_ver = nftnl_expr_alloc("immediate");

	if (ex_ver == NULL) {
		perror("OOM");
		exit(EXIT_FAILURE);
	}
	nftnl_expr_set_u32(ex_ver, NFTNL_EXPR_IMM_DREG,    dreg);
	nftnl_expr_set_u32(ex_ver, NFTNL_EXPR_IMM_VERDICT, verdict);
	nftnl_expr_set(ex_ver,     NFTNL_EXPR_IMM_CHAIN, chain, sizeof(chain));

	nftnl_rule_add_expr(rule, ex_ver);
}

//----------------------------------------------------------------------------
struct nftnl_rule *nft_add_rule_impl (uint8_t family, const char *table,
				   const char *chain, int8_t proto, uint16_t dprt, uint16_t sprt,
				   const char* sadd, const char* dadd, Int8 verdict, char *argv,const char *handle)
{
	struct mnl_socket *nl;
	struct nlmsghdr *nlh;
	struct mnl_nlmsg_batch *batch;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nftnl_rule *rule = NULL;
	uint64_t handle_num;
    uint16_t dport = 0, sport = 0;
    uint8_t saddr[sizeof(struct in6_addr)];     /* Packets coming from */
    uint8_t daddr[sizeof(struct in6_addr)];     /* Packets coming to  */
	uint32_t seq = time(NULL);
	int ret;

	rule = nftnl_rule_alloc();
	if (rule == NULL) {
		perror("OOM");
		exit(EXIT_FAILURE);
	}

	nftnl_rule_set_u32(rule, NFTNL_RULE_FAMILY, family);
	nftnl_rule_set(rule, NFTNL_RULE_TABLE, table);
	nftnl_rule_set(rule, NFTNL_RULE_CHAIN, chain);

	if ( handle != NULL ) {
		handle_num = atoll(handle);
		nftnl_rule_set_u64(rule, NFTNL_RULE_POSITION, handle_num);
	}

	if( proto != UNKNOWN)
	nft_add_payload(rule, NFT_PAYLOAD_NETWORK_HEADER, NFT_REG_1, offsetof(struct iphdr, protocol), sizeof(uint8_t));
	nft_add_cmp(rule, NFT_REG_1, NFT_CMP_EQ, &proto, sizeof(uint8_t));


	if ( dprt != NONE )			/* Declaring dport */
	{
		dport = htons(dprt);
		nft_add_payload(rule, NFT_PAYLOAD_TRANSPORT_HEADER, NFT_REG_1, offsetof(struct tcphdr, dest), sizeof(uint16_t));
		nft_add_cmp(rule, NFT_REG_1, NFT_CMP_EQ, &dport, sizeof(uint16_t));
	}

	if( dadd != NULL )			/* Declaring des ip */
	{
		nft_add_payload(rule, NFT_PAYLOAD_NETWORK_HEADER, NFT_REG_1, offsetof(struct iphdr, daddr), sizeof(uint32_t));
		inet_pton(AF_INET, dadd, daddr);
		nft_add_cmp(rule, NFT_REG_1, NFT_CMP_EQ, daddr, sizeof(uint32_t));
	}

	if( sprt != NONE )			/* Declaring sport */
	{
		sport = htons(sprt);
		nft_add_payload(rule, NFT_PAYLOAD_TRANSPORT_HEADER, NFT_REG_1, offsetof(struct tcphdr, source), sizeof(uint16_t));
		nft_add_cmp(rule, NFT_REG_1, NFT_CMP_EQ, &sport, sizeof(uint16_t));
	}

	if( sadd != NULL )			/* Declaring src ip */
	{
		nft_add_payload(rule, NFT_PAYLOAD_NETWORK_HEADER, NFT_REG_1, offsetof(struct iphdr, saddr), sizeof(uint32_t));
		inet_pton(AF_INET, sadd, saddr);
		nft_add_cmp(rule, NFT_REG_1, NFT_CMP_EQ, saddr, sizeof(uint32_t));
	}
	if(verdict != UNKNOWN)			/* Add verdict */
	{
		if( verdict == NFT_JUMP)
		{
			nft_add_verdict(rule, NFT_REG_VERDICT, &proto, sizeof(uint8_t), argv, verdict);
		}
		else
		nft_add_verdict(rule, NFT_REG_VERDICT, &proto, sizeof(uint8_t), chain, verdict);
	}

	nl = mnl_socket_open(NETLINK_NETFILTER);
	if (nl == NULL) {
		perror("mnl_socket_open");
		exit(EXIT_FAILURE);
	}

	if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
		perror("mnl_socket_bind");
		exit(EXIT_FAILURE);
	}

	batch = mnl_nlmsg_batch_start(buf, sizeof(buf));

	nftnl_batch_begin(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);

	nlh = nftnl_rule_nlmsg_build_hdr(mnl_nlmsg_batch_current(batch),
			NFT_MSG_NEWRULE,
			nftnl_rule_get_u32(rule, NFTNL_RULE_FAMILY),
			NLM_F_APPEND|NLM_F_CREATE|NLM_F_ACK, seq++);

	nftnl_rule_nlmsg_build_payload(nlh, rule);
	nftnl_rule_free(rule);
	mnl_nlmsg_batch_next(batch);

	nftnl_batch_end(mnl_nlmsg_batch_current(batch), seq++);
	mnl_nlmsg_batch_next(batch);

	ret = mnl_socket_sendto(nl, mnl_nlmsg_batch_head(batch),
				mnl_nlmsg_batch_size(batch));
	if (ret == -1) {
		perror("mnl_socket_sendto");
		exit(EXIT_FAILURE);
	}

	mnl_nlmsg_batch_stop(batch);

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	if (ret == -1) {
		perror("mnl_socket_recvfrom");
		exit(EXIT_FAILURE);
	}

	ret = mnl_cb_run(buf, ret, 0, mnl_socket_get_portid(nl), NULL, NULL);
	if (ret < 0) {
		perror("mnl_cb_run");
		exit(EXIT_FAILURE);
	}

	mnl_socket_close(nl);
	return rule;
}
//---------------------------------------------------------------------
void ctor_nft_opt(nft_t *self)
{
	self->nft_add_table             = nft_add_table_impl;
	self->nft_add_chain				= nft_add_chain_impl;
	self->nft_add_rule				= nft_add_rule_impl;

}
