/*
 * Main.c
 *
 *  Created on: Jul 16, 2019
 *      Author: hasnaa
 */

#include "cli.h"

struct cli_s curr_cli_obj;

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

printf("argc = %d\n",argc);

	cli_ctor(&curr_cli_obj);			/*cli constructor*/

	curr_cli_obj.cli_run(&curr_cli_obj, argc, argv);

}
