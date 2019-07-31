/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/
#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>

#include "../types.h"

#include "helper_functions.h"

#include "time_logger.h"
#include "BufferSerializer.h"
#include "mms_socket.h"

#define MAX_SIZE 4096 // Maximum message length
#define DAEMON_NAME "Trapeze"

/** log handler signature*/
typedef void ( *log_handler_t ) ( log_level_t level, const char* message );

/* types */
typedef struct log_s log_t;
typedef struct outputConfig outputConfig_t;

struct outputConfig
{
  bool_t ToMMS;
  bool_t ToConsole;
  bool_t ToSyslogs;

};

/** log structure object structure*/
struct log_s
{
  //-----------------------------------------------------------------
  // encapsulated member functions
  //-----------------------------------------------------------------
  /** Default logger implementation */
  void ( *log_handler_default )( log_t* self, log_level_t level, const char* format, ... );

  /** Logger implementation for MMS compliance */
  void ( *log_handler_MMS )( log_t* self, log_level_t level, unsigned int lineNumber, const char* sourceFile, const char* format, ... );

  /** Returns the current log level. */
  log_level_t ( *get_log_level )( log_t* self );

  /** Sets the log level to the specified value. */
  void ( *set_log_level )( log_t* self, log_level_t level );

  /** Add a custom log callback, use NULL to reset default handler */
  void ( *set_log_handler )( log_t* self, log_handler_t handler );

  /** Destructor*/
  void ( *log_dtor )( log_t* self );

  //-----------------------------------------------------------------
  // private variables - do not access directly
  //-----------------------------------------------------------------
  mms_socket_t     m_socket;          /**< communicating socket. */
  log_level_t      m_cur_max_level;          /*current log level limit {traces above will be logged}*/
  log_handler_t    m_customized_log_handler; /*non-default log handler*/
  bool_t           m_MMStarget;
  bool_t           m_ToConsole;
  bool_t           m_ToSyslogs;
  const char*      m_moduleName;
  bool_t           m_socket_rxValid;

};

/** Constructor*/
void log_ctor( log_t* self, outputConfig_t outputConfig, const char* moduleName );

/** Helper functions declarations */
char* messageFormat( const char* format, va_list ap );
void LogMessage ( log_t* self, log_level_t level, UInt32 lineNumber, const char* sourceFile, char* Message );

void printToConsole( LogEntry_t LogEntry );
void printToSysLogs( LogEntry_t LogEntry, log_level_t level );

bool_t log_create ( UInt16 port, const char* moduleName, outputConfig_t outputConfig, const Int8* version );
/** current logger instance*/
extern struct log_s current_logger_obj;


/*#define RETAILMSG(level, ...) do {\
    if ((level)<= current_logger_obj.get_log_level(&current_logger_obj))\
    {\
      current_logger_obj.log_handler_default((&current_logger_obj),(level), __VA_ARGS__);\
    }\
} while(0)*/


#define RETAILMSG(level, ...) do {\
      current_logger_obj.log_handler_MMS((&current_logger_obj),(level),__LINE__, __FILE__, __VA_ARGS__);\
} while(0)

// To disable logs
/*
#define RETAILMSG(level, ...) do {\
} while(0)
*/
#endif /* LOGGER_H_ */
