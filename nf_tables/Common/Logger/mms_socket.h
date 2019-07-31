/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/
/**
 * @file mms_socket.h
 * @author Amardeep Grewal
 * @date 15.11.2017
 * @brief File contains mms_socket interface.
 *
 */
#ifndef MMS_SOCKET_H_
#define MMS_SOCKET_H_

#include <fcntl.h>

#include "console_log.h"
#include "mms_socket_rx.h"

/**
 * @brief mms socket type definition.
 */
typedef struct mms_socket_s mms_socket_t;

/**
 * @brief This is responsible for UDP socket communication.
 */
struct mms_socket_s
{
  //-----------------------------------------------------------------
  // encapsulated member functions
  //-----------------------------------------------------------------
  /** Basic methods */
  /**
   * @brief Send packet to remote endpoint.
   * @param self Pointer to itself i.e. equivalent of 'this' in C++.
   * @param local_port Local port to bind.
   * @return true[success] or false[failure].
   */
  bool_t ( *mms_socket_open )( mms_socket_t* self, UInt16 local_port );
  bool_t ( *mms_socket_close )( mms_socket_t* self );
  bool_t ( *mms_socket_connect )( mms_socket_t* self );
  bool_t ( *mms_socket_disconnect )( mms_socket_t* self );
  bool_t ( *mms_socket_isconnected )( mms_socket_t* self );

  /** Destructor*/
  void ( *mms_socket_dtor )( mms_socket_t* self );

  /**
   * @brief Send packet to remote endpoint.
   * @param self Pointer to itself i.e. equivalent of 'this' in C++.
   * @param message Pointer to message data to send.
   * @param len Message length.
   * @return true[success] or false[failure].
   */
  bool_t ( *mms_socket_send )( mms_socket_t* self, const UInt8* message, const UInt32 len );
  //-----------------------------------------------------------------
  // private variables - avoid direct access
  //-----------------------------------------------------------------
  mms_socket_rx_t        m_rxr_task;     /**< receiver thread. */
  fd_t                   m_socket_fd;    /**< socket file descriptor. */
  UInt16                 m_local_port;   /**< local port on which socket is binded. */
  bool_t                 m_isConnected;
};

/**
 * @brief Constructor of mms_socket.
 * @param self Pointer to itself i.e. equivalent of 'this' in C++.
 * @return NULL.
 */
void mms_socket_ctor( mms_socket_t* self );


#endif /* MMS_SOCKET_H_ */
