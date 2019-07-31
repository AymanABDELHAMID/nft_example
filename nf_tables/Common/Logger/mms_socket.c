/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/
/**
 * @file mms_socket.c
 * @author Amardeep Grewal
 * @date 15.11.2017
 * @brief File contains mms_socket implementation.
 *
 */
#include "mms_socket.h"
#include <unistd.h>   /**< for close. */
#include <errno.h>

//-----------------------------------------------------------------------------
bool_t mms_socket_open_impl( mms_socket_t* self, UInt16 local_port )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );
  bool_t ret = E_TRUE;

  self->m_local_port = local_port;

  return ret;
}

//-----------------------------------------------------------------------------
bool_t mms_socket_close_impl( mms_socket_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );
  bool_t ret = E_TRUE;

  return ret;
}

//-----------------------------------------------------------------------------
bool_t mms_socket_connect_impl( mms_socket_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );
  CONSOLE_ERR_CHK_NULL_RET( &self->m_rxr_task.mms_socket_rx_start );
  bool_t ret = E_TRUE;
  Int32 optval = 1;

  /*prepare data*/
  struct sockaddr_in sockaddr_in;
  sockaddr_in.sin_family = AF_INET;
  sockaddr_in.sin_addr.s_addr = INADDR_ANY;
  sockaddr_in.sin_port = htons( self->m_local_port );

  /*create the socket*/
  if ( ( self->m_socket_fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
  {
    CONSOLE_MSG( ZONE_ERROR, "Failed to create socket for port<%d> <%s>", self->m_local_port, strerror( errno ) );
    ret = E_FALSE;
  }
  /*set the keepalive option*/
  else if ( setsockopt( self->m_socket_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(Int32) ) < 0 )
  {
    CONSOLE_MSG( ZONE_ERROR, "Failed to set socket option keep_alive <%s>", strerror( errno ) );
    ret = E_FALSE;
  }
  /*bind the socket*/
  else if ( bind( self->m_socket_fd, ( struct sockaddr* ) &sockaddr_in, sizeof(struct sockaddr) ) < 0 )
  {
    CONSOLE_MSG( ZONE_ERROR, "Failed to bind socket for port<%d> <%s>", self->m_local_port, strerror( errno ) );
    ret = E_FALSE;
  }
  /*listen on the socket if the connection is TCP*/
  else if ( listen( self->m_socket_fd, 128 ) < 0 )
  {
    CONSOLE_MSG( ZONE_ERROR, "Failed to listen on  port<%d> <%s>", self->m_local_port, strerror( errno ) );
    ret = E_FALSE;
  }
  /*start receiver thread*/
  else if ( self->m_rxr_task.mms_socket_rx_start( &self->m_rxr_task, self->m_socket_fd ) == E_FALSE )
  {
    CONSOLE_MSG( ZONE_ERROR, "Failed to start receiver thread for port<%d>", self->m_local_port );
    ret = E_FALSE;
  }
  else
  {
    self->m_isConnected = E_TRUE;
    CONSOLE_MSG( ZONE_DEBUG, "Success starting receiver thread for port<%d>", self->m_local_port );
  }
  return ret;
}

//-----------------------------------------------------------------------------
bool_t mms_socket_disconnect_impl( mms_socket_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );
  CONSOLE_ERR_CHK_NULL_RET( self->m_rxr_task.mms_socket_rx_stop );
  bool_t ret = E_TRUE;

  self->m_isConnected = E_FALSE;
  /*stop the thread*/
  if ( self->m_rxr_task.mms_socket_rx_stop( &self->m_rxr_task ) == E_FALSE )
  {
    CONSOLE_MSG( ZONE_ERROR, "Failed to stop receiver thread for port<%d>", self->m_local_port );
    ret = E_FALSE;
  }

  /*disconnect socket*/
  close( self->m_socket_fd );
  close( self->m_rxr_task.m_socket_incoming_fd );
  self->m_socket_fd   = INVALID_HANDLE;
  self->m_rxr_task.m_socket_incoming_fd = INVALID_HANDLE;

  return ret;
}

//-----------------------------------------------------------------------------
bool_t mms_socket_isconnected_impl( mms_socket_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );

  return (self->m_socket_fd > 0) && ( self->m_isConnected );
}

//-----------------------------------------------------------------------------
bool_t mms_socket_send_impl( mms_socket_t* self, const UInt8* message, const UInt32 len )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );
  CONSOLE_ERR_CHK_NULL_RET( message );

  CONSOLE_ASSERT_COND_RET( ( len != 0 ), "Error sending data:: nothing to send" );
  CONSOLE_ASSERT_COND_RET( ( self->mms_socket_isconnected( self ) ), "Error sending data:: socket not connected" );
  CONSOLE_ASSERT_COND_RET( ( self->m_rxr_task.mms_socket_rx_incomingValid( &self->m_rxr_task ) ), "Error sending data:: incoming mms socket not connected" );

  bool_t ret = E_FALSE;

  /* Set the socket to be non blocking */
  if ( fcntl( self->m_rxr_task.m_socket_incoming_fd, F_SETFL, fcntl( self->m_rxr_task.m_socket_incoming_fd, F_GETFL, 0 ) | O_NONBLOCK ) < 0 )
  {
    CONSOLE_MSG( ZONE_ERROR, "Setting socket to non blocking failed <%s>", strerror( errno ) );
  }
  /* Do we have an active connection ? */
  else if ( self->m_rxr_task.m_socket_active )
  {
    /*send data*/
    Int32 byte_sent = send( self->m_rxr_task.m_socket_incoming_fd, message, len, 0 );
    if ( byte_sent < 0 )
    {
      CONSOLE_MSG( ZONE_ERROR, "Error sending data:: error code<%s> [%s][%s()][%d]", strerror( errno ), __FILE__, __FUNCTION__, __LINE__ );
    }
    else if ( byte_sent != len )
    {
      CONSOLE_MSG( ZONE_ERROR, "Error sending data:: Bytes sent <%d> not equal to buffer length <%d>", byte_sent, len );
    }
    else
    {
      CONSOLE_MSG( ZONE_DATA,  "Sent bytes[%d]", byte_sent );
      ret = E_TRUE;
    }
  }
  else
  {
    CONSOLE_MSG( ZONE_ERROR,  "No active connections<%s>", strerror( errno ) );
  }

  /*If the send fails disconnect the socket */
  if ( !ret )
  {
    CONSOLE_MSG( ZONE_DATA, "Send failed, disconnecting" );
    self->mms_socket_disconnect( self );
  }
  return ret;
}

//-----------------------------------------------------------------------------
void mms_socket_dtor_impl( mms_socket_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );
}

//-----------------------------------------------------------------------------
void mms_socket_ctor( mms_socket_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );

  /*initialize data members*/
  mms_socket_rx_ctor( &self->m_rxr_task );
  self->m_socket_fd   = INVALID_HANDLE;
  self->m_local_port  = 0;
  self->m_isConnected = E_FALSE;

  /*save method members*/
  self->mms_socket_dtor         = mms_socket_dtor_impl;
  self->mms_socket_open         = mms_socket_open_impl;
  self->mms_socket_close        = mms_socket_close_impl;
  self->mms_socket_connect      = mms_socket_connect_impl;
  self->mms_socket_disconnect   = mms_socket_disconnect_impl;
  self->mms_socket_isconnected  = mms_socket_isconnected_impl;
  self->mms_socket_send         = mms_socket_send_impl;
}
