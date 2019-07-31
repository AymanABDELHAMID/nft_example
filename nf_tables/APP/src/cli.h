/*
 * cli.h
 *
 *  Created on: Jul 19, 2019
 *      Author: hasnaa
 */

#ifndef CLI_H_
#define CLI_H_

#include "packetClassification.h"

typedef struct cli_s cli_t;

/* nf_tables commands */
enum {
	CLI_OPTION_ADD_TABLE = 10,
	CLI_OPTION_ADD_CHAIN,
	CLI_OPTION_ADD_RULE
};

enum {
	CLI_OPTION_SETUP 	= 20,
	CLI_OPTION_ADD_RULES
};
/* Generic options */
enum {
	  CLI_OPTION_EXIT
};

struct cli_s
{
	 void  ( *cli_run )      ( cli_t* self, int argc, char *argv[] );

	 pktcls_t				 m_pktcls;	/*Every thing related to packet classification*/
};


void cli_ctor ( cli_t* self );

extern struct cli_s curr_cli_obj;

#endif /* CLI_H_ */
