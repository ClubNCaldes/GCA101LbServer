
/**
 *******************************************************************************************
 * @file        EthLocBuffer.c
 * @ingroup     EthLocBuffer
 * @attention
 *******************************************************************************************
 */

/*
 *******************************************************************************************
 * Standard include files
 *******************************************************************************************
 */

#include <inttypes.h>
#include <string.h>
#include "UserIo.h"
#include "ln_buf.h"
#include "uip_arp.h"
#include "ln_interface.h"
#include "EthLocBuffer.h"

/*
 *******************************************************************************************
 * Macro definitions
 *******************************************************************************************
 */

/* *INDENT-OFF* */

#define ETH_LOC_BUFFER_MAX_LOCONET_TX     20          /**< Max number of tries * LN_TX_RETRIES_MAX !! */

#define SENDINGIDLE 0
#define SENDINGDATA 1
#define SENDINGOK 2

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


static LnBuf      EthLocBufferLocoNet;                /**< Loconet buffer */
static uint8_t    EthLocBufferTcpIpRocRail;           /**< Index for TcpIp transmit */
static uint8_t    EthLocBufferUipTimerCounter;        /**< Timer counter for uip handling */
static uint8_t    EthLocBufferUipArpTimerCounter;     /**< Timer counter for uip Arp handling */
/* *INDENT-ON* */
static uint8_t IdenBytes = 0;
static uint8_t IdenFound = 0;
static LnBuf        LnTxBuffer;

static uint8_t sendOk=0;
static uint8_t sendData=0;

static char dataData[70];
static uint8_t sendDataLen;

const char sentokstring[]="SENT OK \r\n";
const char digitMap[] = "0123456789abcdef";
const char receiveString[] = "RECEIVE ";

static uint8_t sendingState=SENDINGIDLE;

/*
 *******************************************************************************************
 * Routines implementation
 *******************************************************************************************
 */
 
 static uint8_t ProcessTCPRxByte(uint8_t offset, uint8_t rxByte)
{
	static uint8_t highNibble = 1;
	static uint8_t loconetByte = 0; 
	static lnMsg   *LoconetPacket;
	LN_STATUS                               TxStatus = LN_DONE;
	uint8_t                                 TxMaxCnt = 0;
	
	
	
	if (highNibble)
	{
		loconetByte = (rxByte - offset) << 4;
		highNibble = 0;
		return 0;
	}
	else
	{
		loconetByte |= (rxByte - offset);
		highNibble = 1;

		/* Add it to the buffer */

		addByteLnBuf( &LnTxBuffer, loconetByte ) ;

		/* Check to see if we have received a complete packet yet */
		LoconetPacket = recvLnMsg( &LnTxBuffer ) ;
		if(LoconetPacket )
		{			
			/* Send the packet received via TCP to the LocoNet*/
			/*LocoNet.send( LoconetPacket ) ;*/				
			do
			{
			   TxStatus = sendLocoNetPacket(LoconetPacket);
			   TxMaxCnt++;
			}
			while ((TxStatus != LN_DONE) && (TxMaxCnt < ETH_LOC_BUFFER_MAX_LOCONET_TX));
		
			return 1;
		}
		return 0;    
	}
}


/**
 *******************************************************************************************
 * @fn         static void EthLocBufferTransmitGlobalPowerOff(void)
 * @brief      Transmit Global PowerOff on the Loconet bus.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */
static void EthLocBufferTransmitGlobalPowerOff(void)
{
   LN_STATUS                               TxStatus = LN_DONE;
   lnMsg                                   LocoNetSendPacket;
   uint8_t                                 TxMaxCnt = 0;
   
   EthLocBufferTcpIpRocRail = 255;

   /* Transmit Power Off on Loconet */
   LocoNetSendPacket.sz.mesg_size = 2;
   LocoNetSendPacket.data[0] = 0x82;
   LocoNetSendPacket.data[1] = 0x7D;

   do
   {
      TxStatus = sendLocoNetPacket(&LocoNetSendPacket);
      TxMaxCnt++;
   }
   while ((TxStatus != LN_DONE) && (TxMaxCnt < ETH_LOC_BUFFER_MAX_LOCONET_TX));

   /* Blink TCPIP led indicating no connection is present to RocRail */
   UserIoSetLed(userIoLed4, userIoLedSetOff);
   UserIoSetLed(userIoLed5, userIoLedSetOff);
   UserIoSetLed(userIoLed4, userIoLedSetBlink);
   UserIoSetLed(userIoLed5, userIoLedSetBlink);
}


/**
 *******************************************************************************************
 * @fn	      void EthLocBufferInit(void)
 * @brief      Init the ENC2860 network chip and init the loconet interface.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

void EthLocBufferInit(void)
{
   /* Init loconet */
   initLocoNet(&EthLocBufferLocoNet);

   /* Set used variables to initial values */   
   EthLocBufferTcpIpRocRail = 255;
   EthLocBufferUipTimerCounter = 0;
   EthLocBufferUipArpTimerCounter = 0;   
   sendOk=0;
   sendData=0;
   sendDataLen=0;
   sendingState=SENDINGIDLE;
}

/**
 *******************************************************************************************
 * @fn	      void EthLocBufferMain(void)
 * @brief      Verify if data is present in the network interface or locnet <br>
 *             interface and process it.
 * @return     None
 * @attention  -
 *******************************************************************************************
 */

void EthLocBufferMain(void)
{
	
   lnMsg  *RxPacket;	
	
    /* If no TCPIP connection discard all received Loconet data */
    if (EthLocBufferTcpIpRocRail == 255)
    {			
        RxPacket = recvLocoNetPacket();
		if (RxPacket)
		{
			UserIoSetLed(userIoLed6, userIoLedSetFlash);
		}
		return;
    }
   
	//Check there is no data pending to send
    if (sendData == 0)
    {		
		RxPacket = recvLocoNetPacket();
		if (RxPacket)
		{				
			uint8_t Lnlen = getLnMsgSize( RxPacket );			
			uint8_t MsgIdx = 8;
			uint8_t Index = 0;
			
			UserIoSetLed(userIoLed6, userIoLedSetFlash);
			
			memcpy(dataData,"RECEIVE ", 8);
			
			for( Index = 0; Index < Lnlen; Index++ )
			{
				dataData[MsgIdx++] = (digitMap[(RxPacket->data[Index] >> 4) & 0xF]);
				dataData[MsgIdx++] =(digitMap[(RxPacket->data[ Index ] & 0xF)]);
				dataData[MsgIdx++] = ' ';
			}			
			MsgIdx--;

			dataData[MsgIdx++] = 0xD;
			dataData[MsgIdx++] = 0xA;
			dataData[MsgIdx] = '\0';
		
			sendDataLen = MsgIdx;   	
			sendData=1;
		}
	}
}

/**
 *******************************************************************************************
 * @fn         void EthLocBufferTcpRcvEthernet(void)
 * @brief      The stack uses calls this function to perform network activities.
 * @return     None
 *******************************************************************************************
 */

void EthLocBufferTcpRcvEthernet(void)
{   
   //char                                    DebugStr[20];

   if (uip_timedout() || uip_aborted() || uip_closed() || uip_closed())
   {
      /* Connection ended */
      EthLocBufferTransmitGlobalPowerOff();
   }

   if (uip_connected())
   {
      /* If connection to RocRail present allow forwarding of Loconet data from Loconet bus to TCPIP. */      
      EthLocBufferTcpIpRocRail = 1;
	  UserIoSetLed(userIoLed4, userIoLedSetOff);
      UserIoSetLed(userIoLed5, userIoLedSetOff);	  
   }

   if (uip_newdata())
   {
		 /* Convertimos de ASCII a formato de loconet para enviar */
		uint8_t indice=0;
		for (indice=0; indice<uip_datalen(); indice++)
		{
			switch (uip_appdata[indice])
			{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					ProcessTCPRxByte(0x30, (uint8_t)uip_appdata[indice]);
					break;
				case 'A':
				case 'B':
				case 'C':
				case 'F':
					ProcessTCPRxByte(0x37, (uint8_t)uip_appdata[indice]);
					break;
				case ' ':
					/* Do nothing */
					break;
				case 'S':
					IdenBytes = 1;
					if (IdenFound == 1)
					{
						IdenFound = 0;
					}
					break;
				case 'E':
					if(IdenFound == 1)
						ProcessTCPRxByte(0x37, (uint8_t)uip_appdata[indice]);
					else
						IdenBytes++;
					break;
				case 'N':
					IdenBytes++;
					break;
				case 'D':
					if(IdenFound == 1)
					{
						ProcessTCPRxByte(0x37, (uint8_t)uip_appdata[indice]);				
					}
					else
					{
						if(IdenBytes == 3)
						{
							IdenFound = 1;
						}
						IdenBytes = 0;
					}
					break;
				default:
					if (IdenFound == 1)
					{
						IdenFound = 0;
						IdenBytes = 0;	
						sendOk+=1;						
					}
					break;
			}
		}
   }

	if (uip_acked())
	{		
		if (sendingState == SENDINGOK)
		{
			sendOk-=1;
			UserIoSetLed(userIoLed4, userIoLedSetOff);
			sendingState=SENDINGIDLE;
		}
		if (sendingState == SENDINGDATA)
		{
			sendData=0;
			UserIoSetLed(userIoLed5, userIoLedSetOff);						
			sendingState=SENDINGIDLE;
		}		
	}
   
	if (uip_poll() || uip_rexmit() || uip_newdata() || uip_acked())
	{
		if (sendingState==SENDINGIDLE)
		{
			/* If Loconet data present from Loconet bus transmit it... */
			if (sendOk>0)
			{	 
				sendingState=SENDINGOK;
			}
			else if (sendData==1)
			{	    
				sendingState=SENDINGDATA;
			}
		}
		
		if (sendingState==SENDINGOK)
		{
			UserIoSetLed(userIoLed4, userIoLedSetOn);		 
			uip_send(sentokstring, 10);
		}
		if (sendingState==SENDINGDATA)
		{
			UserIoSetLed(userIoLed5, userIoLedSetOn);
			uip_send(dataData, sendDataLen);
		}
		
		
		
	}
}
