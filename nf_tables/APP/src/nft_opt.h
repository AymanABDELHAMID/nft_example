/*
 * libnftnl.h
 *
 *  Created on: Jul 15, 2019
 *      Author: hasnaa
 */

#ifndef NFT_OPT_H_
#define NFT_OPT_H_

#include "common.h"

typedef struct nft_s  nft_t;


struct nft_s
{
 struct nftnl_table * (*nft_add_table) 		( char *tbl, uint32_t family);
 struct nftnl_chain *(*nft_add_chain)       ( int argc, char *argv[], uint32_t family, bool_t bchn);
 struct nftnl_rule *(*nft_add_rule)			( uint8_t family, const char *table,
		   	   	   	   	   	   	   	   	   	  const char *chain, int8_t proto, uint16_t dprt,
											  uint16_t sprt, const char* sadd, const char* dadd,
											  Int8 verdict,char *argv, const char *handle);

};


void ctor_nft_opt( nft_t *self);

extern struct  nft_s curr_nft_obj;

#endif /* NFT_OPT_H_ */
