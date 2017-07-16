
/**
 *******************************************************************************************
 * @file		EthLocBuffer.h
 * @ingroup	    EthLocBuffer
 * @defgroup	EthLocBuffer EthLocBuffer : Loconet / UDP processing. 
 * @brief		EthLocBuffer Process the data between the network interface and Loconet interface. 
 *******************************************************************************************
 */
#ifndef ETH_LOC_BUF
#define ETH_LOC_BUF

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */
#include <inttypes.h>

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

#define ETH_LOC_BUFFER_BUFFER_TCP_PORT    5550        /**< Port to listen / write */

/*
 *******************************************************************************************
 * Types
 *******************************************************************************************
  */

/*
 *******************************************************************************************
 * Variables
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Prototypes
 *******************************************************************************************
 */

extern void                             EthLocBufferInit(void);
extern void                             EthLocBufferMain(void);
extern void                             EthLocBufferTcpRcvEthernet(void);

#endif   // ETH_LOC_BUF
