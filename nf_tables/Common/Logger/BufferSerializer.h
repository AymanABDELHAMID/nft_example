/*
 * BufferSerializer.h
 *
 *  Created on: Nov 28, 2017
 *      Author: taimir
 */

#ifndef BUFFERSERIALIZER_H_
#define BUFFERSERIALIZER_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../types.h"
#include "console_log.h"

typedef struct
{
  UInt8* dataBuffer;
  UInt32 bufferSize;
} messageBuffer_t;

messageBuffer_t serializeHeader( LogHeader_t LogHeader );
messageBuffer_t serializeBuffer( LogEntry_t LogEntry );
bool_t Serialize( UInt8* pBuffer, UInt32* pBufferPosition, const void* pValue, UInt32 valueSize );
bool_t SerializeString( UInt8* pBuffer, UInt32* pBufferPosition, const void* pValue, UInt32 valueSize );
UInt32 computeHeaderSize ();


#endif /* BUFFERSERIALIZER_H_ */
