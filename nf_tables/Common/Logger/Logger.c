/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/
#include "../helper_macros.h"
#include "Logger.h"
#include <stdarg.h>
#include <stdio.h>


/** current logger instance*/
struct log_s current_logger_obj;

/** signal handler structures*/
struct sigaction log_act;
struct sigaction log_def;
struct sigaction log_ign;

//-----------------------------------------------------------------------------
static void log_sighandler ( int signum, siginfo_t* info, void* ptr )
{
  //RETAILMSG (ZONE_WARNING,"Received signal number %d from PID %lu \n", signum, (unsigned long) info->si_pid);
  CONSOLE_MSG ( ZONE_WARNING, "Logger Received signal number %d from PID %lu \n", signum, ( unsigned long ) info->si_pid );
  switch ( signum )
  {
  case SIGPIPE:
    CONSOLE_MSG ( ZONE_WARNING, "Signals ignored" );
    sigaction ( signum, &log_ign, NULL );
    raise( signum );
    break;
  default:
    sigaction ( signum, &log_def, NULL );
    raise( signum );
    break;
  }

}

//-----------------------------------------------------------------------------
void log_handler_default_impl( log_t* self, log_level_t level, const Int8* format, ... )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );
  if ( self->m_cur_max_level < level )
  {
    return;
  }

  if ( self->m_customized_log_handler )
  {
    Int8 message[128];
    va_list ap;
    va_start( ap, format );
    vsnprintf( message, sizeof(message), format, ap );
    va_end( ap );
    self->m_customized_log_handler( level, message );
  }
  else
  {
    va_list ap;
    va_start( ap, format );
    vfprintf( stdout, format, ap );
    vfprintf( stdout, "\n", ap );
    va_end( ap );
    fflush( stdout );
  }
}

//-----------------------------------------------------------------------------
void log_handler_MMS_impl( log_t* self, log_level_t level, UInt32 lineNumber, const Int8* sourceFile, const Int8* format, ... )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );
  CONSOLE_ERR_CHK_NULL_RET_NONE( sourceFile );

  va_list ap;
  va_start( ap, format );

  Int8* messageBuffer = messageFormat( format, ap );
  LogMessage ( self, level, lineNumber, sourceFile, messageBuffer );

  free( messageBuffer );

  va_end( ap );

}

//-----------------------------------------------------------------------------
Int8* messageFormat( const Int8* format, va_list ap )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  Int8* buf = ( Int8* ) malloc( MAX_SIZE );
  Int32 length = vsnprintf( buf, MAX_SIZE, format, ap );
  if ( length >= 0 )
  {
    if ( length == MAX_SIZE )
    {
      CONSOLE_MSG( ZONE_WARNING, "Log message length reached size limit" );
    }

  }
  else
  {
    CONSOLE_MSG( ZONE_ERROR, "Could not allocate buffer" );
  }

  return buf;
}

//-----------------------------------------------------------------------------
void LogMessage ( log_t* self, log_level_t level, UInt32 lineNumber, const Int8* sourceFile, Int8* Message )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );

  LogEntry_t LogEntry;

  // Generate missing data in logEntry

  preciseTime_t currentTime = GetCurrentTime(); //time_t and milliseconds
  const Int8* zoneName = GetDebugZoneName( level );
  UInt8 logMessageType = 9; // fixed now for raw data transmission

  // Gather all the information required in a struct
  LogEntry.localTime = currentTime.localTime;
  LogEntry.milliseconds = currentTime.milliseconds;
  LogEntry.logMessageType = logMessageType;
  LogEntry.lineNumber = lineNumber;
  LogEntry.sourceFile = sourceFile;
  LogEntry.zoneInfo = zoneName;
  LogEntry.module = self->m_moduleName;
  LogEntry.logMessage = Message;

  // Convert LogEntry to serialized buffer
  messageBuffer_t outputBuffer = serializeBuffer( LogEntry );


  if ( level <= self->m_cur_max_level )
  {
    // send buffer over socket
    if ( self->m_MMStarget )
    {
      if ( self->m_socket.mms_socket_isconnected( &self->m_socket ) == E_TRUE )
      {
        CONSOLE_MSG( ZONE_DEBUG, "Socket is connected" );
        if ( self->m_socket.m_rxr_task.mms_socket_rx_incomingValid( &self->m_socket.m_rxr_task ) == E_TRUE )
        {
          CONSOLE_MSG( ZONE_DEBUG, "Incoming socket is valid" );
          self->m_socket_rxValid = E_TRUE;
          self->m_socket.mms_socket_send( &self->m_socket, outputBuffer.dataBuffer, outputBuffer.bufferSize );
        }
        else if ( self->m_socket_rxValid == E_TRUE )
        {
          CONSOLE_MSG( ZONE_WARNING, "Incoming MMS socket not connected" );
          self->m_socket_rxValid = E_FALSE;
        }
      }
      else
      {
        // Try to connect
        self->m_socket.mms_socket_connect( &self->m_socket );
      }
    }
    // Print LogEntry to console
    if ( self->m_ToConsole )
    {
      printToConsole ( LogEntry );
    }
    // Print LogEntry to syslogs
    if ( self->m_ToSyslogs )
    {
      printToSysLogs ( LogEntry, level );
    }
  }
  // clean memory
  free( outputBuffer.dataBuffer );
}

//-----------------------------------------------------------------------------
void printToConsole( LogEntry_t LogEntry )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  Int8 timeBuffer[26];
  struct tm* tm_info = gmtime( &( LogEntry.localTime ) );
  strftime( timeBuffer, 26, "%Y:%m:%d %H:%M:%S", tm_info );

  printf( "%s.%03u |%10s | %s | %s | \n",
          timeBuffer, LogEntry.milliseconds,
          LogEntry.module,
          LogEntry.zoneInfo,
          LogEntry.logMessage );
}

//-----------------------------------------------------------------------------
void printToSysLogs( LogEntry_t LogEntry, log_level_t level )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );

  UInt32 syslogLevel;
  Int8 timeBuffer[26];
  struct tm* tm_info = gmtime( &( LogEntry.localTime ) );
  strftime( timeBuffer, 26, "%Y:%m:%d %H:%M:%S", tm_info );
  //Set our Logging Mask and open the Log
  setlogmask( LOG_UPTO( LOG_DEBUG ) );
  openlog( DAEMON_NAME, LOG_NDELAY | LOG_PID, LOG_LOCAL0 ); //LOG_LOCAL0 defined in syslog.conf

  switch ( level )
  {
  case ZONE_ERROR:
    syslogLevel = LOG_ERR;
    break;

  case ZONE_WARNING:
    syslogLevel = LOG_WARNING;
    break;

  case ZONE_INFO:
    syslogLevel = LOG_INFO;
    break;

  case ZONE_DEBUG:
    syslogLevel = LOG_DEBUG;
    break;

  default:
    syslogLevel = LOG_NOTICE;

  }

  syslog( syslogLevel, "%s.%03u |%10s | %c | %s | \n",
          timeBuffer, LogEntry.milliseconds,
          LogEntry.module,
          LogEntry.zoneInfo[0],
          LogEntry.logMessage );
}

//-----------------------------------------------------------------------------
log_level_t get_log_level_impl( log_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_VAL( self, ZONE_DEBUG );
  return self->m_cur_max_level;
}

//-----------------------------------------------------------------------------
void set_log_level_impl( log_t* self, log_level_t level )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );
  self->m_cur_max_level = level;
}

//-----------------------------------------------------------------------------
void set_log_handler_impl( log_t* self, log_handler_t handler )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );
  self->m_customized_log_handler = handler;
}

//-----------------------------------------------------------------------------
void log_dtor_impl( log_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );
}

//-----------------------------------------------------------------------------
void log_ctor( log_t* self, outputConfig_t outputConfig, const Int8* moduleName )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );
  CONSOLE_ERR_CHK_NULL_RET_NONE( moduleName );

  /*initialize data members*/
  self->m_cur_max_level          = ZONE_INFO;
  self->m_customized_log_handler = NULL;
  self->m_MMStarget        = outputConfig.ToMMS;
  self->m_ToConsole        = outputConfig.ToConsole;
  self->m_ToSyslogs        = outputConfig.ToSyslogs;
  self->m_moduleName       = moduleName;
  self->m_socket_rxValid   = E_FALSE;

  mms_socket_ctor( &self->m_socket );

  /*save method members*/
  self->log_handler_default = log_handler_default_impl;
  self->log_handler_MMS     = log_handler_MMS_impl;
  self->get_log_level       = get_log_level_impl;
  self->set_log_level       = set_log_level_impl;
  self->set_log_handler     = set_log_handler_impl;
  self->log_dtor            = log_dtor_impl;
}

//-----------------------------------------------------------------------------
bool_t log_create ( UInt16 port, const Int8* moduleName, outputConfig_t outputConfig, const Int8* version )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( moduleName );

  bool_t Ret = E_FALSE;

  /*initialize the logger*/
  log_ctor( &current_logger_obj, outputConfig, moduleName );

  current_logger_obj.m_socket.m_rxr_task.version = version;

  if ( !current_logger_obj.m_socket.mms_socket_open( &current_logger_obj.m_socket, port ) )
  {
    return E_FALSE;
  }
  if ( !current_logger_obj.m_socket.mms_socket_connect( &current_logger_obj.m_socket ) )
  {
    return E_FALSE;
  }

  memset ( &log_act, 0, sizeof(log_act) );
  memset ( &log_act, 0, sizeof(log_def) );

  log_act.sa_sigaction = log_sighandler;
  log_act.sa_flags = SA_SIGINFO;
  log_def.sa_handler = SIG_DFL;
  log_ign.sa_handler = SIG_IGN;

  sigaction ( SIGKILL, &log_act, NULL );
  sigaction ( SIGTERM, &log_act, NULL );
  sigaction ( SIGHUP, &log_act, NULL );
  sigaction ( SIGINT, &log_act, NULL );
  sigaction ( SIGQUIT, &log_act, NULL );
  sigaction ( SIGPIPE, &log_act, NULL );

  return Ret = E_TRUE;
}
