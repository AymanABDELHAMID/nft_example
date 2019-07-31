/*
 * MmsLogHeaderGenerator.h
 *
 *  Created on: Nov 28, 2017
 *      Author: taimir
 */

#ifndef MMSLOGHEADERGENERATOR_H_
#define MMSLOGHEADERGENERATOR_H_

#include "BufferSerializer.h"
#include "time_logger.h"
#include "console_log.h"

messageBuffer_t GenerateHeader ( const Int8* version );


#endif /* MMSLOGHEADERGENERATOR_H_ */
