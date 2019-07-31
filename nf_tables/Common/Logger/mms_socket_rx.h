/*--------------------------------------------------------------------------------
Trapeze Switzerland GmbH, CH-8212 Neuhausen am Rh  Copyright (C) 2017
---------------------------------------------------------------------------------*/
/**
 * @file mms_socket_rx.h
 * @author Amardeep Grewal
 * @date 15.11.2017
 * @brief File contains mms_socket_rx interface.
 *
 */
#ifndef MMS_SOCKET_RX_H_
#define MMS_SOCKET_RX_H_

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "console_log.h"
#include "MmsLogHeaderGenerator.h"

/**
 * @brief receiver thread type definition.
 */
typedef struct mms_socket_rx_s mms_socket_rx_t;

/**
 * @brief This is responsible for socket receiver thread.
 */
struct mms_socket_rx_s
{
  //-----------------------------------------------------------------
  // encapsulated member functions
  //-----------------------------------------------------------------
  /** Basic methods */
  bool_t ( *mms_socket_rx_start )( mms_socket_rx_t* self, fd_t socket_fd );
  bool_t ( *mms_socket_rx_stop )( mms_socket_rx_t* self );

  /** Destructor*/
  void ( *mms_socket_rx_dtor )( mms_socket_rx_t* self );

  /** @brief Runnable routine.*/
  void* ( *mms_socket_rx_run )( void* self );

  /** @brief TCP-based connection methods*/
  bool_t ( *mms_socket_rx_accept )( mms_socket_rx_t* self );
  bool_t ( *mms_socket_rx_poll )( mms_socket_rx_t* self );
  bool_t ( *mms_socket_rx_valid )( mms_socket_rx_t* self );
  bool_t ( *mms_socket_rx_incomingValid )( mms_socket_rx_t* self );

  //-----------------------------------------------------------------
  // private variables - avoid direct access
  //-----------------------------------------------------------------
  bool_t                m_run;         /**< tracks if thread is running or not. */
  pthread_t             m_ID;          /**< thread ID. */
  fd_t                  m_socket_fd;   /**< socket file descriptor. */
  fd_t                  m_socket_incoming_fd;   /**< socket file descriptor for incoming connection. */
  bool_t                m_socket_active;  /**< socket active flag for logging. */
  Int8*                 version;   /**<version of the SW component running the logger */
};

/**
 * @brief Constructor of mms_socket_rx.
 * @param self Pointer to itself i.e. equivalent of 'this' in C++.
 * @return NULL.
 */
void mms_socket_rx_ctor( mms_socket_rx_t* self );

#endif /* MMS_SOCKET_RX_H_ */
