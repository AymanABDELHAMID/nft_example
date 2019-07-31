/*
 * packetClassification.c
 *
 *  Created on: Jul 16, 2019
 *      Author: hasnaa
 */

#include "packetClassification.h"


//--------------------------------------------------------------------
uint32_t pkt_detect_family_impl(char *argv,const char *option)
{
	uint16_t family;

	if (strcmp(argv, "ip") == 0)
		family = NFPROTO_IPV4;
	else if (strcmp(argv, "ip6") == 0)
		family = NFPROTO_IPV6;
	else if (strcmp(argv, "bridge") == 0)
		family = NFPROTO_BRIDGE;
	else if (strcmp(argv, "arp") == 0)
		family = NFPROTO_ARP;
	else {
		fprintf(stderr, "Unknown %s: ip, ip6, inet, arp\n", option);
		exit(EXIT_FAILURE);
	}

return family;
}

//-------------------------------------------------------------------
uint8_t pkt_parse_l4proto_impl(char *argv)		/* argv[4] */
{
	if (strcmp(argv, "udp") == 0)
		return IPPROTO_UDP;
	else if (strcmp(argv, "ip") == 0)
		return IPPROTO_IP;
	else if (strcmp(argv, "icmp") == 0)
		return IPPROTO_ICMP;
	else if (strcmp(argv, "tcp") == 0)
		return IPPROTO_TCP;
	else if (strcmp(argv, "igmp") == 0)
		return IPPROTO_IGMP;
	else if (strcmp(argv, "ipip") == 0)
		return IPPROTO_IPIP;
	else if (strcmp(argv, "egp") == 0)
		return IPPROTO_EGP;
	else if (strcmp(argv, "pup") == 0)
		return IPPROTO_PUP;
	else if (strcmp(argv, "idp") == 0)
		return IPPROTO_IDP;
	else if (strcmp(argv, "tp") == 0)
		return IPPROTO_TP;
	else if (strcmp(argv, "dccp") == 0)
		return IPPROTO_DCCP;
	else if (strcmp(argv, "ipv6") == 0)
		return IPPROTO_IPV6;
	else if (strcmp(argv, "rsvp") == 0)
		return IPPROTO_RSVP;
	else if (strcmp(argv, "rsvp") == 0)
		return IPPROTO_RSVP;
	else if (strcmp(argv, "gre") == 0)
		return IPPROTO_GRE;
	else if (strcmp(argv, "esp") == 0)
		return IPPROTO_ESP;
	else if (strcmp(argv, "ah") == 0)
		return IPPROTO_AH;
	else if (strcmp(argv, "mtp") == 0)
		return IPPROTO_MTP;
	else if (strcmp(argv, "beetph") == 0)
		return IPPROTO_BEETPH;
	else if (strcmp(argv, "encap") == 0)
		return IPPROTO_ENCAP;
	else if (strcmp(argv, "pim") == 0)
		return IPPROTO_PIM;
	else if (strcmp(argv, "comp") == 0)
		return IPPROTO_COMP;
	else if (strcmp(argv, "sctp") == 0)
		return IPPROTO_SCTP;
	else if (strcmp(argv, "udplite") == 0)
		return IPPROTO_UDPLITE;
	else if (strcmp(argv, "mpls") == 0)
		return IPPROTO_MPLS;
	else if (strcmp(argv, "row") == 0)
		return IPPROTO_RAW;
	else {
		fprintf(stderr, "Unknown layer 4 protocol\n");
		return IPPROTO_TCP;

	}
}

//--------------------------------------------------------------
int8_t pkt_str2verdict_impl(const char *verdict)
{
	if (strcmp(verdict, "accept") == 0)
	{
		return NF_ACCEPT;
	}
	else if (strcmp(verdict, "drop") == 0)
	{
		return NF_DROP;
	}
	else if (strcmp(verdict, "return") == 0)
	{
		return NFT_RETURN;
	}
	else if (strcmp(verdict, "jump") == 0)
	{
		return NFT_JUMP;
	}
	else if (strcmp(verdict, "goto") == 0)
	{
		return NFT_GOTO;
	}
	return UNKNOWN;
}

//--------------------------------------------------------------
/* the setup will add a table called filter
 * inside the ip family and two base chains for
 * input and output traffic with priority 0.
 */
void pkt_setup_impl(pktcls_t *self, int argc, char *argv[])
{
	 /* Detect family name */
	  argv[1] = "ip";
	  self->family = self->pkt_detect_family( argv[1], "family");
			  /* Add new table */
	  argv[2] = "filter";
	  self->m_nft_opt.nft_add_table( argv[2], self->family);

	  	  	  /* base chain "input" */
	  argv[3] = "input";		/* Chain name */
	  argv[4] = "input";		/* Hook name */
	  argv[5] = "0";			/* priority */
	  self->m_nft_opt.nft_add_chain( argc, argv, self->family, E_TRUE);

	  	  	  /* base chain "output" */
	  argv[3] = "output";		/* Chain name */
	  argv[4] = "output";		/* Hook name */
	  argv[5] = "0";			/* priority */
	  self->m_nft_opt.nft_add_chain( argc, argv, self->family, E_TRUE);

	  	  	  /* none base chain "tcp-chain" */
	  argv[3] = "tcp-chain";		/* Chain name */
	  argv[4] = NULL;			    /* Hook name */
	  argv[5] = NULL;			    /* priority */
	  self->m_nft_opt.nft_add_chain( argc, argv, self->family, E_FALSE);

	  	  	  /* none base chain "udp-chain" */
	  argv[3] = "udp-chain";		/* Chain name */
	  argv[4] = NULL;			    /* Hook name */
	  argv[5] = NULL;			    /* priority */
	  self->m_nft_opt.nft_add_chain( argc, argv, self->family, E_FALSE);

	  	  	  /* none base chain "icmp-chain" */
	  argv[3] = "icmp-chain";		/* Chain name */
	  argv[4] = NULL;			    /* Hook name */
	  argv[5] = NULL;			    /* priority */
	  self->m_nft_opt.nft_add_chain( argc, argv, self->family, E_FALSE);

}

//--------------------------------------------------------------
void pkt_classification_rules_impl( pktcls_t *self )
{				/* add some rules in input chain */
	self->m_nft_opt.nft_add_rule( self->family, "filter", "input", IPPROTO_UDP, NONE, NONE, NULL, NULL, NFT_JUMP, "udp-chain", NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "input", IPPROTO_TCP, NONE, NONE, NULL, NULL, NFT_JUMP, "tcp-chain", NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "input", IPPROTO_ICMP, NONE, NONE, NULL, NULL, NFT_JUMP, "icmp-chain", NULL);

	self->m_nft_opt.nft_add_rule( self->family, "filter", "udp-chain", IPPROTO_UDP, HTTP_PORT, NONE, NULL, NULL, NF_DROP, NULL, NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "udp-chain", IPPROTO_UDP, HTTPS_PORT, NONE, NULL, NULL, NF_ACCEPT,NULL, NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "icmp-chain", IPPROTO_ICMP, NONE, NONE, NULL, NULL, NF_ACCEPT, NULL, NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "tcp-chain", IPPROTO_TCP, SSH_PORT, NONE, DADDR, NULL, NF_ACCEPT,NULL, NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "udp-chain", IPPROTO_UDP, HTTPS_PORT, NONE, NULL, SADDR, NF_DROP, NULL, NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "icmp-chain", IPPROTO_ICMP, NONE, NONE, NULL, SADDR, NF_DROP, NULL, NULL);

				/* add some rules in output chain */
	self->m_nft_opt.nft_add_rule( self->family, "filter", "output", IPPROTO_UDP, NONE, NONE, NULL, NULL, NFT_JUMP, "udp-chain", NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "output", IPPROTO_TCP, NONE, NONE, NULL, NULL, NFT_JUMP, "tcp-chain", NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "output", IPPROTO_ICMP, NONE, NONE, NULL, NULL, NFT_JUMP, "icmp-chain", NULL);

	self->m_nft_opt.nft_add_rule( self->family, "filter", "udp-chain", IPPROTO_UDP, HTTP_PORT, NONE, NULL, NULL, NF_DROP, NULL, NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "udp-chain", IPPROTO_UDP, HTTPS_PORT, NONE, NULL, NULL, NF_ACCEPT, NULL, NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "icmp-chain", IPPROTO_ICMP, NONE, NONE, NULL, NULL, NF_ACCEPT, NULL, NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "tcp-chain", IPPROTO_TCP, SSH_PORT, NONE, DADDR, NULL, NF_DROP, NULL, NULL);
	self->m_nft_opt.nft_add_rule( self->family, "filter", "udp-chain", IPPROTO_UDP, HTTPS_PORT, NONE, NULL, SADDR, NF_DROP, NULL, NULL);

}
//--------------------------------------------------------------
void pkt_ctor( pktcls_t *self )
{
	self->family 			       = NFPROTO_IPV4;	/*default family ip*/

	self->pkt_detect_family		   = pkt_detect_family_impl;
	self->pkt_parse_l4proto		   = pkt_parse_l4proto_impl;
	self->pkt_str2verdict		   = pkt_str2verdict_impl;
	self->pkt_setup				   = pkt_setup_impl;
	self->pkt_classification_rules = pkt_classification_rules_impl;

	ctor_nft_opt(&self->m_nft_opt);
}


