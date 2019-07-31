/*
 * time.h
 *
 *  Created on: Nov 27, 2017
 *      Author: taimir
 */

#ifndef TIME_H_
#define TIME_H_

#include <stdio.h>
#include <time.h>
#include <math.h>
#include "../types.h"
#include "console_log.h"

typedef struct
{
  time_t localTime;
  UInt32 milliseconds;
} preciseTime_t;

preciseTime_t GetCurrentTime( void );


#endif /* TIME_H_ */
