/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/

#include "console_log.h"

void log_handler ( log_level_t level, const char* format, ... )
{
  //const char* zoneName = GetDebugZoneName(level);

  va_list ap;
  va_start( ap, format );
  //printf("| %c : ",zoneName[0]); // Adds confusion
  printf( "CONS: " );
  vfprintf( stdout, format, ap );
  vfprintf( stdout, "\n", ap );
  va_end( ap );
  fflush( stdout );
}
