/*
 * time.c
 *
 *  Created on: Nov 27, 2017
 *      Author: taimir
 */

#include "time_logger.h"

preciseTime_t GetCurrentTime()
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  preciseTime_t CurrentTime;
  struct timespec spec;

  clock_gettime( CLOCK_REALTIME, &spec );

  CurrentTime.localTime = spec.tv_sec;
  CurrentTime.milliseconds = spec.tv_nsec / 1.0e6 ;

  return CurrentTime;
}
