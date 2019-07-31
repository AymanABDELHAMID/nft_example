/*
 * helper_functions.c
 *
 *  Created on: Dec 1, 2017
 *      Author: taimir
 */

#include "helper_functions.h"

//-----------------------------------------------------------------------------
const char* GetDebugZoneName ( UInt32 zoneIndex )
{
  int i = 0;
  if ( zoneIndex == ZONE_ALL )
  {
    return "ALL";
  }
  for ( ; i < 15; i++ )
  {
    if ( zoneIndex & ( 1 << i ) )
    {
      break;
    }
  }
  return DEBUG_ZONE_ARRAY[i];
}
