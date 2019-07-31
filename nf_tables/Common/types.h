/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/
#ifndef TYPES_H_
#define TYPES_H_

#include <memory.h>   /**< for memset/memcpy. */
#include <string.h>   /**< for string manipulations. */
#include <time.h>     /** < for time manipulation. */

//! UInt8: Type guaranteed to be 8-bit unsigned.
typedef unsigned char   UInt8;
//! UInt16: Type guaranteed to be 16-bit unsigned.
typedef unsigned short    UInt16;
//! UInt32: Type guaranteed to be 32-bit unsigned.
typedef unsigned int    UInt32;

//! Int8: Type guaranteed to be 8-bit signed.
typedef char        Int8;
//! Int16: Type guaranteed to be 16-bit signed.
typedef short       Int16;
//! Int32: Type guaranteed to be 32-bit signed.
typedef int         Int32;

//! fd_t: File descriptor.
typedef int         fd_t;
#define INVALID_HANDLE (-1)

enum
{
  E_TRUE  = 1,
  E_FALSE = 0
};
#define NOT(arg) ((arg == E_TRUE)? E_FALSE : E_TRUE)
typedef int bool_t;

/** Log Entry structure */
typedef struct
{
  time_t localTime;
  UInt32 milliseconds;
  UInt8 logMessageType;
  UInt32 lineNumber;
  const char* sourceFile;
  const char* zoneInfo;
  const char* module;
  char* logMessage;
} LogEntry_t;

/** Header Structure */
typedef struct
{
  time_t localTime;
  UInt32 milliseconds;
  UInt8 logVersion;
  UInt8 inputType;
  UInt16 logOptions;
  const char* fileNamePattern;
} LogHeader_t;

/* Predefined log_levels in arrays */
#define DEBUG_ZONE_ARRAY (const char*[]){"Error",\
    "Warning", \
    "Info ", \
    "Trace", \
    "Init", \
    "Debug", \
    "Function", \
    "ctor", \
    "Data", \
    "Data1", \
    "Data2", \
    "Data3", \
    "Data4", \
    "Data5", \
    "Squirrel", \
    "DebugConsole", \
}

/** Pre-defined log levels*/
typedef enum
{
  ZONE_ERROR = 0x0001,
  ZONE_WARNING = 0x0002,
  ZONE_INFO = 0x0004,
  ZONE_TRACE = 0x0008,
  ZONE_INIT = 0x0010,
  ZONE_DEBUG = 0x0020,
  ZONE_FUNCTION = 0x0040,
  ZONE_CTOR = 0x0080,
  ZONE_DATA = 0x0100,
  ZONE_DATA1 = 0x0200,
  ZONE_DATA2 = 0x0400,
  ZONE_DATA3 = 0x0800,
  ZONE_DATA4 = 0x1000,
  ZONE_DATA5 = 0x2000,
  ZONE_SQUIRREL = 0x4000,
  ZONE_DEBUG_CONSOLE = 0x8000,
  ZONE_ALL = 0xffff
} log_level_t;

#endif /* TYPES_H_ */
