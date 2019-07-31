/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/
/**
 * @file mms_socket_rx.c
 * @author Amardeep Grewal
 * @date 15.11.2017
 * @brief File contains mms_socket_rx implementation.
 *
 */
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "mms_socket_rx.h"

/**
 * @brief Receiver buffer maximum size.
 */
#define MAX_RX_BUFFER_SIZE     (1024)

//-------------------causes error in pthread_create----------------------------------------------------------
bool_t mms_socket_rx_start_impl( mms_socket_rx_t* self, fd_t socket_fd )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );
  CONSOLE_ERR_CHK_NULL_RET( self->mms_socket_rx_run );
  bool_t ret = E_TRUE;

//  self->m_socket_fd = socket_fd;
//
//  /*check if already running*/
//  if ( self->m_run == E_TRUE )
//  {
//    CONSOLE_MSG( ZONE_ERROR, "Failed to start socket receiver thread, already running status" );
//    ret = E_FALSE;
//  }
//  /*create thread*/
//  else if ( pthread_create( &self->m_ID, NULL, self->mms_socket_rx_run, self ) )
//  {
//    CONSOLE_MSG( ZONE_ERROR, "Failed to create socket receiver thread" );
//    ret = E_FALSE;
// }
  return ret;
}

//-----------------------------------------------------------------------------
bool_t mms_socket_rx_stop_impl( mms_socket_rx_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );
  bool_t ret = E_TRUE;

  self->m_run = E_FALSE;

  return ret;
}

//-----------------------------------------------------------------------------
void mms_socket_rx_dtor_impl( mms_socket_rx_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );
}

//-----------------------------------------------------------------------------
void* mms_socket_rx_run_TCP_impl( void* self_in )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_VAL( self_in, NULL );
  CONSOLE_MSG( ZONE_DEBUG, "mms_socket_rx::Run() starts" );
  mms_socket_rx_t* self = ( mms_socket_rx_t* )self_in;
  CONSOLE_ERR_CHK_NULL_RET_VAL( self->mms_socket_rx_accept, NULL );

  UInt8 buffer [MAX_RX_BUFFER_SIZE];

  // Wait for connection
  if ( self->mms_socket_rx_accept( self ) )
  {
    Int32 sendReturn = 0;
    messageBuffer_t headerBuffer = GenerateHeader ( self->version );
    sendReturn = send( self->m_socket_incoming_fd, headerBuffer.dataBuffer, headerBuffer.bufferSize, 0 );

    if ( sendReturn < 0 )
    {
      CONSOLE_MSG( ZONE_ERROR, "Failed to send header buffer, error code<%s> [%s][%s()][%d]", strerror( errno ), __FILE__, __FUNCTION__, __LINE__ );
    }
    else if ( sendReturn != headerBuffer.bufferSize )
    {
      CONSOLE_MSG( ZONE_ERROR, "Error sending data:: Bytes sent <%d> not equal to buffer length <%d>", sendReturn, headerBuffer.bufferSize );
    }
    else
    {
      CONSOLE_MSG( ZONE_DEBUG, "Header buffer sent successfully, bytes sent<%d>", sendReturn );
      self->m_socket_active = E_TRUE;

      /*executive loop*/
      while ( self && self->m_run == E_TRUE )
      {
        /*wait for any incoming packet*/
        Int32 byteReceived = recv( self->m_socket_fd, buffer, MAX_RX_BUFFER_SIZE, MSG_WAITALL );
        if ( byteReceived < 0 )
        {
          CONSOLE_MSG( ZONE_ERROR, "Error receiving data (error in connection ), error code<%d>", byteReceived );
          self->m_socket_active = E_FALSE;
        }
        usleep( 500 );
      }

    }
    // clean header buffer
    free( headerBuffer.dataBuffer );
  }

  CONSOLE_MSG( ZONE_INFO, "Connected to MMS" );
  return NULL;
}

//-----------------------------------------------------------------------------
bool_t mms_socket_rx_accept_impl( mms_socket_rx_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );
  CONSOLE_ERR_CHK_NULL_RET( self->mms_socket_rx_valid );
  CONSOLE_ERR_CHK_NULL_RET( self->mms_socket_rx_incomingValid );
  CONSOLE_ERR_CHK_NULL_RET( self->mms_socket_rx_poll );

  bool_t bRetVal = E_FALSE;

  if ( self->mms_socket_rx_valid( self ) == E_TRUE &&
       self->mms_socket_rx_incomingValid( self ) == E_FALSE &&
       self->mms_socket_rx_poll( self ) )
  {
    self->m_socket_incoming_fd = accept( self->m_socket_fd, NULL, NULL );
    if ( self->mms_socket_rx_incomingValid( self ) == E_TRUE )
    {
      bRetVal = E_TRUE;
    }
    else
    {
      CONSOLE_MSG( ZONE_ERROR, "Failed to accept connection" );
    }
  }
  return bRetVal;
}

//-----------------------------------------------------------------------------
bool_t mms_socket_rx_incomingValid_impl( mms_socket_rx_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );

  return (self->m_socket_incoming_fd > 0);
}

//-----------------------------------------------------------------------------
bool_t mms_socket_rx_valid_impl( mms_socket_rx_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );

  return (self->m_socket_fd > 0);
}

//-----------------------------------------------------------------------------
bool_t mms_socket_rx_poll_impl( mms_socket_rx_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET( self );

  return E_TRUE; // dummy function for now, not implemented
}
//-----------------------------------------------------------------------------
void mms_socket_rx_ctor( mms_socket_rx_t* self )
{
  CONSOLE_MSG( ZONE_FUNCTION, __FUNCTION__ );
  CONSOLE_ERR_CHK_NULL_RET_NONE( self );

  /*initialize data members*/
  self->m_run                   = E_FALSE;
  self->m_socket_fd             = INVALID_HANDLE;
  self->m_socket_incoming_fd    = INVALID_HANDLE;
  self->m_socket_active         = E_FALSE;
  self->version                 = NULL;

  /*save method members*/
  self->mms_socket_rx_dtor            = mms_socket_rx_dtor_impl;
  self->mms_socket_rx_start           = mms_socket_rx_start_impl;
  self->mms_socket_rx_stop            = mms_socket_rx_stop_impl;
  self->mms_socket_rx_accept          = mms_socket_rx_accept_impl;
  self->mms_socket_rx_poll            = mms_socket_rx_poll_impl;
  self->mms_socket_rx_valid           = mms_socket_rx_valid_impl;
  self->mms_socket_rx_incomingValid   = mms_socket_rx_incomingValid_impl;
  self->mms_socket_rx_run             = mms_socket_rx_run_TCP_impl;

}
