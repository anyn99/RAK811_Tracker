/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Generic low level driver for GPS receiver

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include <board.h>

/*!
 * FIFO buffers size
 */
//#define FIFO_TX_SIZE                                128
//#define FIFO_RX_SIZE                                128

//uint8_t TxBuffer[FIFO_TX_SIZE];

#define UBX_SYNC_CHAR1 	0xB5
#define UBX_SYNC_CHAR2 	0x62

enum UbxClassId
{
	CFG = 0x06
};

enum UbxMsgId
{
	RXM = 0x11,
	PM2 = 0x3B
};

uint8_t UbxMsg[255];
uint8_t UbxMsgSize = 0;
uint8_t UbxMsgCalculatedSize = 0;

/*!
 * \brief Buffer holding the  raw data received from the gps
 */
uint8_t NmeaString[128];

/*!
 * \brief Maximum number of data byte that we will accept from the GPS
 */
uint8_t NmeaStringSize = 0;

Gpio_t GpsPowerEn;
Gpio_t GpsPps;

PpsTrigger_t PpsTrigger;

bool IsInPowerSafe = false;

void GpsMcuSendUbxMsg(uint8_t ClassId, uint8_t MsgId, uint8_t *Buffer, uint16_t Size)
{
	unsigned int Offset = 0;
	uint8_t SBuffer[64];
	uint8_t CK_A = 0;
	uint8_t CK_B = 0;

	assert_param( (Size+8) <= sizeof(SBuffer) );

	SBuffer[Offset++] = UBX_SYNC_CHAR1;
	SBuffer[Offset++] = UBX_SYNC_CHAR2;
	SBuffer[Offset++] = ClassId;
	SBuffer[Offset++] = MsgId;
	SBuffer[Offset++] = (uint8_t) Size & 0xFF;     //Length, 16bit unsigned little endian
	SBuffer[Offset++] = (uint8_t) (Size>>8) & 0xFF;

	if (Buffer)
	{
		memcpy(&SBuffer[Offset], Buffer, Size);
		Offset += Size;
	}

	unsigned int i;
	for (i = 2; i < Offset; i++) //checksum from ClassId till end of payload (8bit fletcher)
	{
		CK_A = CK_A + SBuffer[i];
		CK_B = CK_B + CK_A;
	}
	SBuffer[Offset++] = CK_A;
	SBuffer[Offset++] = CK_B;

	UartMcuPutBuffer(&GpsUart, SBuffer, Offset);
}

void GpsMcuGetSleepConfig( void )
{
	GpsMcuSendUbxMsg(CFG, PM2, NULL, 0);
}

void GpsMcuSetSleepConfig( void )
{
	/*
	unsigned int BufferSize = 44;
	uint8_t Buffer[BufferSize];
	uint32_t PSMflags = ; //MTODO: all variables need to be turned to little endian!
	uint32_t updatePeriod = ;
	uint32_t searchPeriod = ;
	uint32_t gridOffset = ;
	uint16_t onTime = ;
	uint16_t minAcqTime = ;

	Buffer[0] = 0x01;			//MsgVersion
	memset(&Buffer[1], 0, 3);
	memcpy(&Buffer[4], &PSMflags, 4);			//PSM flags
	memcpy(&Buffer[8], &updatePeriod, 4);			//Position update period in ms
	memcpy(&Buffer[12], &searchPeriod, 4);				//Acquisition retry period in ms
	memcpy(&Buffer[16], &gridOffset, 4);				//Grid Offset in ms
	memcpy(&Buffer[20], &onTime, 2);			//On Time after first fix in seconds
	memcpy(&Buffer[22], &minAcqTime, 2);			//minimal search time in seconds

	memset(&Buffer[24], 0, (44-24));

	GpsSendUbxMsg(CFG, PM2, Buffer, BufferSize);
	*/
}

void GpsMcuEnablePubxPosition( void )
{

	uint8_t SBuffer[13] = "$PUBX,00*33\r\n"; //"$PUBX,40,POSITION,1,1,1,1,1,0*03\r\n";

	UartMcuPutBuffer(&GpsUart, SBuffer, 13);
    DelayMs(100);
}

void GpsMcuActivateSleep( void )
{
	unsigned int BufferSize = 2;
	uint8_t Buffer[BufferSize];

	Buffer[0] = 8;     //reserved value
	Buffer[1] = 1;     //enable power safe mode 0=off 1=on

	GpsMcuSendUbxMsg(CFG, RXM, Buffer, BufferSize);
}

bool GpsMcuCheckAck(uint8_t ClassId, uint8_t MsgId, uint8_t *Buffer, uint16_t Size)
{
	uint8_t Ack[] = {UBX_SYNC_CHAR1, UBX_SYNC_CHAR2, 0x05, 0x01, 2, 0, ClassId, MsgId};
	if (memcmp(Buffer, Ack, (Size -2)) == 0)
	{
		return true;
	}

	return false;
}

/*
void GpsMcuActivateSleep( void )
{
	unsigned int BufferSize = 10;
	uint8_t Buffer[BufferSize];
	uint8_t CK_A = 0;
	uint8_t CK_B = 0;

	Buffer[0] = UBX_SYNC_CHAR1;
	Buffer[1] = UBX_SYNC_CHAR2;
	Buffer[2] = CFG;
	Buffer[3] = RXM;
	Buffer[4] = 2;     //Langth, 16bit unsigned little endian
	Buffer[5] = 0;
	Buffer[6] = 8;     //reserved value
	Buffer[7] = 1;     //enable power safe mode 0=off 1=on

	int i;
	for (i = 2; i < 8; i++) //checksum from ClassId till end of payload (8bit fletcher)
	{
		CK_A = CK_A + Buffer[i];
		CK_B = CK_B + CK_A;
	}
	Buffer[8] = CK_A;
	Buffer[9] = CK_B;

	UartMcuPutBuffer(&GpsUart, Buffer, BufferSize);
	DelayMs(1000);
}
*/

void GpsMcuOnPpsSignal( void )
{
#ifdef GPS_PPS
    bool parseData = false;

    GpsPpsHandler( &parseData );

    if( parseData == true )
    {
        UartMcuInit( &GpsUart, GPS_UART, GPS_UART_TX, GPS_UART_RX );
        UartMcuConfig( &GpsUart, RX_ONLY, 9600, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );
    }
#endif
}

void GpsMcuInvertPpsTrigger( void )
{
#if 1
    if( PpsTrigger == PpsTriggerIsRising )
    {
        PpsTrigger = PpsTriggerIsFalling;
        GpioSetInterrupt( &GpsPps, IRQ_FALLING_EDGE, IRQ_VERY_LOW_PRIORITY, &GpsMcuOnPpsSignal );
    }
    else
    {
        PpsTrigger = PpsTriggerIsRising;
        GpioSetInterrupt( &GpsPps, IRQ_RISING_EDGE, IRQ_VERY_LOW_PRIORITY, &GpsMcuOnPpsSignal );
    }
#endif
}

void GpsMcuInit( void )
{
    NmeaStringSize = 0;
    PpsTrigger = PpsTriggerIsFalling;

    GpioInit( &GpsPowerEn, GPS_POWER_ON_PIN, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );

    GpioInit( &GpsPps, GPS_PPS_PIN, PIN_INPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioSetInterrupt( &GpsPps, IRQ_FALLING_EDGE, IRQ_VERY_LOW_PRIORITY, &GpsMcuOnPpsSignal );

    //FifoInit( &GpsUart.FifoRx, RxBuffer, FIFO_RX_SIZE );
    //GpsUart.IrqNotify = GpsMcuIrqNotify;

    GpsMcuStart( );

    UartMcuInit( &GpsUart, GPS_UART, GPS_UART_TX, GPS_UART_RX );

#ifndef GPS_PPS     
    UartMcuConfig( &GpsUart, RX_TX, 9600, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );
#else
    UartMcuConfig( &GpsUart, TX_ONLY, 9600, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );
#endif   

}

void GpsMcuReInit( void )
{
	UartMcuInit( &GpsUart, GPS_UART, GPS_UART_TX, GPS_UART_RX );

#ifndef GPS_PPS
	UartMcuConfig( &GpsUart, RX_TX, 9600, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );
#else
	UartMcuConfig( &GpsUart, TX_ONLY, 9600, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );
#endif
}

void GpsMcuStart( void )
{
    GpioWrite( &GpsPowerEn, 1 );    // power up the GPS
}

void GpsMcuStop( void )
{
    GpioWrite( &GpsPowerEn, 0 );    // power down the GPS
}

void GpsMcuProcess( void )
{
	uint8_t data = 0;
	NmeaStringSize = 0;

	UartMcuDeInit(&GpsUart);
	while( UartMcuGetChar( &GpsUart, &data ) == 0)
	{
		if (UbxMsgSize)
		{
			UbxMsg[UbxMsgSize++] = data;
			if (UbxMsgSize == 2 && data != UBX_SYNC_CHAR2)
			{
				NmeaString[NmeaStringSize++] = UBX_SYNC_CHAR1;
				NmeaString[NmeaStringSize++] = data;
			    UbxMsgSize = 0;
			}
			else if (UbxMsgSize == 6)
			{
				uint16_t LenField = UbxMsg[4] | (UbxMsg[5]<<8);
				UbxMsgCalculatedSize = 8 + LenField;
			}
			else if (UbxMsgSize > 6 && UbxMsgSize == UbxMsgCalculatedSize)
			{
				printf("Received UBX: ");
				dump_hex2str(UbxMsg, UbxMsgCalculatedSize);
				if (GpsMcuCheckAck(CFG, RXM, UbxMsg, UbxMsgSize)) IsInPowerSafe = true;
				UbxMsgSize = 0;
				UbxMsgCalculatedSize = 0;
			}
			continue;
		}

		if ( (data == UBX_SYNC_CHAR1 && UbxMsgSize == 0) || UbxMsgSize >=255)
		{
			UbxMsgSize = 0;
			UbxMsgCalculatedSize = 0;
			UbxMsg[UbxMsgSize++] = data;
			continue;
		}

		if( ( data == '$' ) || ( NmeaStringSize >= 127 ) )
		{
			NmeaStringSize = 0;
		}

		NmeaString[NmeaStringSize++] = ( int8_t )data;

		if( NmeaString[0] == '$' && data == '\n' )
		{
			NmeaString[NmeaStringSize++] = '\0';
			//printf("%s\r\n", NmeaString);
			//printf("%d\r\n",NmeaStringSize);
			GpsParseGpsData( ( int8_t* )NmeaString, NmeaStringSize );
		}
	}
	FifoFlush(&GpsUart.FifoRx);
	UartMcuInit( &GpsUart, GPS_UART, GPS_UART_TX, GPS_UART_RX );
	UartMcuConfig( &GpsUart, RX_TX, 9600, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );

	if (!IsInPowerSafe) GpsMcuActivateSleep();

	//GpsMcuGetSleepConfig();
}

void GpsMcuIrqNotify( UartNotifyId_t id )
{
#if (0)
    uint8_t data;
    if( id == UART_NOTIFY_RX )
    {
        if( UartMcuGetChar( &GpsUart, &data ) == 0 )
        {
        	/*
        	if (UbxMsgSize)
        	{
        		UbxMsg[UbxMsgSize++] = data;
        		if (UbxMsgSize == 6)
        		{
        			uint16_t LenField = UbxMsg[4] | (UbxMsg[5]<<8);
        			UbxMsgCalculatedSize = 8 + LenField;
        		}
        		else if (UbxMsgSize > 6 && UbxMsgSize == UbxMsgCalculatedSize)
        		{
        			printf("Received UBX: ");
        			dump_hex2str(UbxMsg, UbxMsgCalculatedSize);
        			UbxMsgSize = 0;
        		}
        	}

        	if ( (data == 0xB5 && UbxMsgSize == 0) || UbxMsgSize >=255)
        	{
        		UbxMsg[UbxMsgSize++] = data;
        	}
        	*/


            if( ( data == '$' ) || ( NmeaStringSize >= 127 ) )
            {
                NmeaStringSize = 0;
                BlockLowPowerDuringTask(true);
            }

            NmeaString[NmeaStringSize++] = ( int8_t )data;

            if( data == '\n' )
            {
                NmeaString[NmeaStringSize++] = '\0';
                printf("%s\r\n", NmeaString);
			    printf("%d\r\n",NmeaStringSize);
                GpsParseGpsData( ( int8_t* )NmeaString, NmeaStringSize );
#ifdef GPS_PPS
                UartMcuDeInit( &GpsUart );
#endif
                BlockLowPowerDuringTask ( false );
            }
        }
    }
#endif
}
