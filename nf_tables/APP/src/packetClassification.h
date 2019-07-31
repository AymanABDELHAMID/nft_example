/*
 * packetClassification.h
 *
 *  Created on: Jul 16, 2019
 *      Author: hasnaa
 */

#ifndef PACKETCLASSIFICATION_H_
#define PACKETCLASSIFICATION_H_

#include "nft_opt.h"

typedef struct pktcls_s pktcls_t;

struct pktcls_s
{
	uint32_t   									 family;

	uint32_t (*pkt_detect_family)				( char *argv, const char *option );
	uint8_t  (*pkt_parse_l4proto)			    ( char *argv );
	int8_t   (*pkt_str2verdict)		     		( const char *verdict );
	void     (*pkt_setup)						( pktcls_t *self, int argc, char *argv[] );
	void	 (*pkt_classification_rules)		( pktcls_t *self );

	nft_t 										 m_nft_opt;
};

void pkt_ctor(pktcls_t *self);

extern struct pktcls_s curr_pktcls_obj;

#endif /* PACKETCLASSIFICATION_H_ */
