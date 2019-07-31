/*
 * cli.c
 *
 *  Created on: Jul 19, 2019
 *      Author: hasnaa
 */

#include "cli.h"

struct cli_s curr_cli_obj;

bool_t bKeepRunning;

//---------------------------------------------------------------
static void printAvailableOpts ( void )
{
  CONSOLE_MSG( ZONE_INFO, "             TEST UTILITY:: nf_tables" );
  CONSOLE_MSG( ZONE_INFO, "+==================================================+" );
  CONSOLE_MSG( ZONE_INFO, "|           Generic nf_tables commands             |" );
  CONSOLE_MSG( ZONE_INFO, "|            ------------------------              |" );
  CONSOLE_MSG( ZONE_INFO, "|   %02d Add tables       |    %02d Add chain          |", CLI_OPTION_ADD_TABLE, CLI_OPTION_ADD_CHAIN );
  CONSOLE_MSG( ZONE_INFO, "|   %02d Add rule         |    %02d exit               |", CLI_OPTION_ADD_RULE, CLI_OPTION_EXIT);
  CONSOLE_MSG( ZONE_INFO, "+==================================================+" );
  CONSOLE_MSG( ZONE_INFO, "|         packet classification commands           |" );
  CONSOLE_MSG( ZONE_INFO, "|            ------------------------              |" );
  CONSOLE_MSG( ZONE_INFO, "|   %02d Required Setup   |    %02d Add rules          |", CLI_OPTION_SETUP, CLI_OPTION_ADD_RULES);
  CONSOLE_MSG( ZONE_INFO, "+==================================================+" );
  printf ( "\n\n" );

}

//----------------------------------------------------------------
static void handle_cli_opts ( Int8* optionStr, cli_t* self, int argc, char *argv[] )
{
  uint8_t proto = IPPROTO_TCP;
  Int32 option = atoi( optionStr );
  switch ( option )
  {
  case CLI_OPTION_ADD_TABLE:
  {
	if (argc != 3) {					/*incorrect command*/
	  fprintf(stderr, "Type right command: %s <family> <name>\n", argv[0]);
	  exit(EXIT_FAILURE);
	}
    self->m_pktcls.family = self->m_pktcls.pkt_detect_family( argv[1], "family");
    self->m_pktcls.m_nft_opt.nft_add_table( argv[2], self->m_pktcls.family);
  }
    break;

  case CLI_OPTION_ADD_CHAIN:
  {
	if (argc != 4 && argc != 6) {
      fprintf(stderr, "Type right command: %s <family> <table> <chain> [<hooknum> <prio>]\n",argv[0]);
	  exit(EXIT_FAILURE);
	}
    self->m_pktcls.family = self->m_pktcls.pkt_detect_family( argv[1], "family");
    self->m_pktcls.m_nft_opt.nft_add_chain( argc, argv, self->m_pktcls.family,E_FALSE);
  }
  break;
  case CLI_OPTION_ADD_RULE:
  {
	if ( argc <= 4 || argc > 6 )
	{
	  if( strcmp(argv[5], "jump" ) )
	  {
		  fprintf(stderr, "Usage: %s <family> <table> <chain> <statement>\n", argv[0] );
		  exit(EXIT_FAILURE);
	  }
	}
    self->m_pktcls.family = self->m_pktcls.pkt_detect_family( argv[1], "family");

    proto = self->m_pktcls.pkt_parse_l4proto( argv[4] );

    if ( argc != 8 )		/* no handle found */
	{
		int8_t verdict = self->m_pktcls.pkt_str2verdict( argv[5] );
    	self->m_pktcls.m_nft_opt.nft_add_rule( self->m_pktcls.family, argv[2], argv[3], proto, NONE, NONE, NULL, NULL, verdict, argv[6], NULL);
	}
	else
	{
		int8_t verdict = self->m_pktcls.pkt_str2verdict(argv[argc - 1]);
	    self->m_pktcls.m_nft_opt.nft_add_rule( self->m_pktcls.family, argv[2], argv[3], proto, NONE, NONE, NULL, NULL, verdict, NULL, argv[7]);
	}
  }

  break;
  case CLI_OPTION_SETUP:
  {
	  self->m_pktcls.pkt_setup( &self->m_pktcls, argc, argv );
  }
  break;
  case CLI_OPTION_ADD_RULES:
  {
	  self->m_pktcls.pkt_classification_rules( &self->m_pktcls );
  }
  break;
  case CLI_OPTION_EXIT:
    bKeepRunning = E_FALSE;
    break;

  default:
     printf("Invalid option < %d >, pleas try again\n", option );
     break;
  }
}

//-----------------------------------------------------------------------------
void cli_run_impl ( cli_t* self, int argc, char *argv[] )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE ( self );
  bKeepRunning = E_TRUE;

  /** Initialize classification */
  pkt_ctor( &self->m_pktcls );

  /* While loop to print available options */
  bKeepRunning = E_TRUE;
  printAvailableOpts();
  while ( bKeepRunning )
  {
    CONSOLE_MSG( ZONE_INFO, "Please enter option:" );

    Int8 optionStr[10] = {0,};
    printf( "->" );
    fgets( optionStr, 10, stdin );
    handle_cli_opts( optionStr, self, argc, argv );
  }
}

//-----------------------------------------------------------------------------
void cli_ctor( cli_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );

  self->cli_run        = cli_run_impl;

}




