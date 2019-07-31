/*
 * MmsLogHeaderGenerator.c
 *
 *  Created on: Nov 28, 2017
 *      Author: taimir
 */

#include "MmsLogHeaderGenerator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

messageBuffer_t GenerateHeader ( const Int8* version )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );

  LogHeader_t LogHeader;
  char str[50];
  char ms[20] = {0,};
  // Generate missing data in logEntry

  preciseTime_t timeStruct = GetCurrentTime(); //time_t and milliseconds
  strftime( str, 27, "%Y:%m:%d %H:%M:%S.", localtime( &( timeStruct.localTime ) ) );
  sprintf( ms, "%03u", LogHeader.milliseconds );
  ms[3] = 0x5f; //_
  ms[4] = 0x76; //v

  strcpy( ( ms + 5 ), version );
  strcat( str, ms );
  strcat( str, " boot reason" );
  // Gather all the information required in a struct
  LogHeader.localTime = timeStruct.localTime;
  LogHeader.milliseconds = timeStruct.milliseconds;
  LogHeader.logVersion = 3;
  LogHeader.inputType = 10;
  LogHeader.logOptions = 0;
  LogHeader.fileNamePattern = str;

  // Convert Header to serialized buffer
  return serializeHeader( LogHeader );

}

