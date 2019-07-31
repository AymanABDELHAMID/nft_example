/*
 * BufferSerializer.c
 *
 *  Created on: Nov 28, 2017
 *      Author: taimir
 */

#include "BufferSerializer.h"

//-----------------------------------------------------------------------------
messageBuffer_t serializeHeader( LogHeader_t LogHeader )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  bool_t bRet = E_FALSE;

  UInt32 fileNamePatternSize = strlen( LogHeader.fileNamePattern ) + 1;


  UInt32 maxBodySize = fileNamePatternSize + // sourceFile
                       sizeof(UInt32) +  // sourceFile size
                       sizeof(UInt16) +  // logoptions
                       sizeof(UInt8);  // logversion

  const UInt32 headerSize = computeHeaderSize();

  UInt32 totalSize = maxBodySize + headerSize;
  UInt8* logBuffer = ( UInt8* ) malloc( totalSize );
  UInt32 BufferPosition = 0;

  bRet = Serialize( logBuffer, &( BufferPosition ),  &( LogHeader.localTime ), sizeof(UInt32) );
  bRet = ( Serialize( logBuffer, &( BufferPosition ), &( LogHeader.milliseconds ), sizeof(UInt16) ) ) && bRet;
  bRet = ( Serialize( logBuffer, &( BufferPosition ), &( LogHeader.inputType ), sizeof(UInt8) ) ) && bRet;
  bRet = ( Serialize( logBuffer, &( BufferPosition ), &( maxBodySize ), sizeof(UInt32) ) ) && bRet;
  bRet = ( SerializeString( logBuffer, &( BufferPosition ), ( LogHeader.fileNamePattern ), fileNamePatternSize ) ) && bRet;
  bRet = ( Serialize( logBuffer, &( BufferPosition ), &( LogHeader.logVersion ), sizeof(UInt8) ) ) && bRet;
  bRet = ( Serialize( logBuffer, &( BufferPosition ), &( LogHeader.logOptions ), sizeof(UInt16) ) ) && bRet;

  messageBuffer_t outputBuffer;

  if ( bRet ) // not used for now
  {
    outputBuffer.dataBuffer = logBuffer;
    outputBuffer.bufferSize = totalSize;
  }
  else
  {
    outputBuffer.dataBuffer = logBuffer;
    outputBuffer.bufferSize = totalSize;
  }

  return outputBuffer;
}

//-----------------------------------------------------------------------------
messageBuffer_t serializeBuffer( LogEntry_t LogEntry )
{
  bool_t bRet = E_FALSE;

  UInt32 sourceFileSize = strlen( LogEntry.sourceFile ) + 1;
  UInt32 zoneInfoSize = strlen( LogEntry.zoneInfo ) + 1;
  UInt32 moduleSize = strlen( LogEntry.module ) + 1;
  UInt32 messageSize = strlen( LogEntry.logMessage ) + 1;

  UInt32 maxBodySize =  sizeof(UInt32) + // lineNumber
                        sizeof(UInt32) + // sourceFileSize
                        sourceFileSize + // sourceFile
                        sizeof(UInt32) + // zoneInfo size
                        zoneInfoSize + // zoneInfo
                        sizeof(UInt32) + // module size
                        moduleSize +  // module
                        sizeof(UInt32) + // message size
                        messageSize;  // message

  const UInt32 headerSize = computeHeaderSize();

  UInt32 totalSize = maxBodySize + headerSize;
  UInt8* logBuffer = ( UInt8* ) malloc( totalSize );
  UInt32 BufferPosition = 0;

  bRet = Serialize( logBuffer, &( BufferPosition ),  &( LogEntry.localTime ), sizeof(UInt32) );
  bRet = ( Serialize( logBuffer, &( BufferPosition ), &( LogEntry.milliseconds ), sizeof(UInt16) ) ) && bRet;
  bRet = ( Serialize( logBuffer, &( BufferPosition ), &( LogEntry.logMessageType ), sizeof(UInt8) ) ) && bRet;
  bRet = ( Serialize( logBuffer, &( BufferPosition ), &( maxBodySize ), sizeof(UInt32) ) ) & bRet;
  bRet = ( Serialize( logBuffer, &( BufferPosition ), &( LogEntry.lineNumber ), sizeof(UInt32) ) ) && bRet;
  bRet = ( SerializeString( logBuffer, &( BufferPosition ), ( LogEntry.sourceFile ), sourceFileSize ) ) && bRet;
  bRet = ( SerializeString( logBuffer, &( BufferPosition ), ( LogEntry.zoneInfo ), zoneInfoSize ) ) && bRet;
  bRet = ( SerializeString( logBuffer, &( BufferPosition ), ( LogEntry.module ), moduleSize ) ) && bRet;
  bRet = ( SerializeString( logBuffer, &( BufferPosition ), ( LogEntry.logMessage ), messageSize ) ) && bRet;

  messageBuffer_t outputBuffer;

  if ( bRet ) // not used for now
  {
    outputBuffer.dataBuffer = logBuffer;
    outputBuffer.bufferSize = totalSize;
  }
  else
  {
    outputBuffer.dataBuffer = logBuffer;
    outputBuffer.bufferSize = totalSize;
  }

  return outputBuffer;
}

//-----------------------------------------------------------------------------
bool_t Serialize( UInt8* pBuffer, UInt32* pBufferPosition, const void* pValue, UInt32 valueSize )
{
  bool_t bValid = E_FALSE;
  if ( NULL != pBuffer )
  {
    if ( NULL != pValue && 0 < valueSize )
    {
      memcpy( pBuffer + * ( pBufferPosition ), pValue, valueSize );
    }
    *( pBufferPosition ) += valueSize;
    bValid = E_TRUE;
  }
  return bValid;
}

//-----------------------------------------------------------------------------
bool_t SerializeString( UInt8* pBuffer, UInt32* pBufferPosition, const void* pValue, UInt32 valueSize )
{
  bool_t bValid = E_FALSE;
  if ( NULL != pBuffer )
  {
    bValid = Serialize( pBuffer, pBufferPosition, &( valueSize ), sizeof(UInt32) );
    bValid = ( Serialize( pBuffer, pBufferPosition, pValue, valueSize ) ) | bValid;
  }
  return bValid;
}

//-----------------------------------------------------------------------------
UInt32 computeHeaderSize()
{
  UInt32 size = sizeof( UInt32 ) + // timestamp
                sizeof( UInt16 ) + // miliseconds
                sizeof( UInt8 ) + // log message type
                sizeof( UInt32 ); // message body size
  return size;
}
