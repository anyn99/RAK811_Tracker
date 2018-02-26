/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Generic driver for any GPS receiver

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <board.h>
#include "gps.h"


#define TRIGGER_GPS_CNT                             10

/* Various type of NMEA data we can receive with the Gps */
const char NmeaDataTypeGPGGA[] = "GPGGA";
const char NmeaDataTypeGPGSA[] = "GPGSA";
const char NmeaDataTypeGPGSV[] = "GPGSV";
const char NmeaDataTypeGPRMC[] = "GPRMC";
const char NmeaDataTypePUBX[] = "PUBX";

/* Value used for the conversion of the position from DMS to decimal */
const int32_t MaxNorthPosition = 8388607;       // 2^23 - 1
const int32_t MaxSouthPosition = 8388608;       // -2^23
const int32_t MaxEastPosition = 8388607;        // 2^23 - 1    
const int32_t MaxWestPosition = 8388608;        // -2^23

tNmeaGpsData NmeaGpsData;

static double HasFix = false;
static double Latitude = 0;
static double Longitude = 0;

static int32_t LatitudeBinary = 0;
static int32_t LongitudeBinary = 0;

static int16_t Altitude = 0xFFFF;
static int8_t SeenSatellites = 0;
static double Hdop = 50;
static double AccuracyMtrs = 100;

static uint32_t PpsCnt = 0;

bool PpsDetected = false;

void GpsPpsHandler( bool *parseData )
{
    PpsDetected = true;
    PpsCnt++;
    *parseData = false;

    if( PpsCnt >= TRIGGER_GPS_CNT )
    {   
        PpsCnt = 0;
        BlockLowPowerDuringTask ( true );
        *parseData = true;
    }
}

void GpsInit( void )
{
    PpsDetected = false;
    GpsMcuInit( );
}

void GpsStart( void )
{
    GpsMcuStart( );
}

void GpsStop( void )
{
    GpsMcuStop( );
}

void GpsProcess( void )
{
    GpsMcuProcess( );
}

bool GpsGetPpsDetectedState( void )
{
    bool state = false;

    BoardDisableIrq( );
    state = PpsDetected;
    PpsDetected = false;
    BoardEnableIrq( );
    return state;
}

bool GpsHasFix( void )
{
    return HasFix;
}

void GpsConvertPositionIntoBinary( void )
{
    long double temp;

    if( Latitude >= 0 ) // North
    {    
        temp = Latitude * MaxNorthPosition;
        LatitudeBinary = temp / 90;
    }
    else                // South
    {    
        temp = Latitude * MaxSouthPosition;
        LatitudeBinary = temp / 90;
    }

    if( Longitude >= 0 ) // East
    {    
        temp = Longitude * MaxEastPosition;
        LongitudeBinary = temp / 180;
    }
    else                // West
    {    
        temp = Longitude * MaxWestPosition;
        LongitudeBinary = temp / 180;
    }
}

void GpsConvertPositionFromStringToNumerical( void )
{
    int i;

    double valueTmp1;
    double valueTmp2;
    double valueTmp3;
    double valueTmp4;

    // Convert the latitude from ASCII to uint8_t values
    for( i = 0 ; i < 10 ; i++ )
    {
        NmeaGpsData.NmeaLatitude[i] = NmeaGpsData.NmeaLatitude[i] & 0xF;
    }
    // Convert latitude from degree/minute/second (DMS) format into decimal
    valueTmp1 = ( double )NmeaGpsData.NmeaLatitude[0] * 10.0 + ( double )NmeaGpsData.NmeaLatitude[1];
    valueTmp2 = ( double )NmeaGpsData.NmeaLatitude[2] * 10.0 + ( double )NmeaGpsData.NmeaLatitude[3];
    valueTmp3 = ( double )NmeaGpsData.NmeaLatitude[5] * 1000.0 + ( double )NmeaGpsData.NmeaLatitude[6] * 100.0 + 
                ( double )NmeaGpsData.NmeaLatitude[7] * 10.0 + ( double )NmeaGpsData.NmeaLatitude[8];
                
    Latitude = valueTmp1 + ( ( valueTmp2 + ( valueTmp3 * 0.0001 ) ) / 60.0 );

    if( NmeaGpsData.NmeaLatitudePole[0] == 'S' )
    {
        Latitude *= -1;
    }
 
    // Convert the longitude from ASCII to uint8_t values
    for( i = 0 ; i < 10 ; i++ )
    {
        NmeaGpsData.NmeaLongitude[i] = NmeaGpsData.NmeaLongitude[i] & 0xF;
    }
    // Convert longitude from degree/minute/second (DMS) format into decimal
    valueTmp1 = ( double )NmeaGpsData.NmeaLongitude[0] * 100.0 + ( double )NmeaGpsData.NmeaLongitude[1] * 10.0 + ( double )NmeaGpsData.NmeaLongitude[2];
    valueTmp2 = ( double )NmeaGpsData.NmeaLongitude[3] * 10.0 + ( double )NmeaGpsData.NmeaLongitude[4];
    valueTmp3 = ( double )NmeaGpsData.NmeaLongitude[6] * 1000.0 + ( double )NmeaGpsData.NmeaLongitude[7] * 100;
    valueTmp4 = ( double )NmeaGpsData.NmeaLongitude[8] * 10.0 + ( double )NmeaGpsData.NmeaLongitude[9];

    Longitude = valueTmp1 + ( valueTmp2 / 60.0 ) + ( ( ( valueTmp3 + valueTmp4 ) * 0.0001 ) / 60.0 );

    if( NmeaGpsData.NmeaLongitudePole[0] == 'W' )
    {
        Longitude *= -1;
    }
}


uint8_t GpsGetLatestGpsPositionDouble( double *lati, double *longi )
{
    uint8_t status = FAIL;
    if( HasFix == true )
    {
        status = SUCCESS;
    }
    else
    {
        GpsResetPosition( );
    }
    *lati = Latitude;
    *longi = Longitude;
    return status;
}

uint8_t GpsGetLatestGpsPositionBinary( int32_t *latiBin, int32_t *longiBin )
{
    uint8_t status = FAIL;

    BoardDisableIrq( );
    if( HasFix == true )
    {
        status = SUCCESS;
    }
    else
    {
        GpsResetPosition( );
    }
    *latiBin = LatitudeBinary;
    *longiBin = LongitudeBinary;
    BoardEnableIrq( );
    return status;
}

int16_t GpsGetLatestGpsAltitude( void )
{
    BoardDisableIrq( );
    if( HasFix == true )
    {    
        Altitude = atoi( NmeaGpsData.NmeaAltitude );
    }
    else
    {
        Altitude = 0xFFFF;
    }
    BoardEnableIrq( );

    return Altitude;
}

uint8_t GpsGetSeenSatellites(void)
{
    BoardDisableIrq( );
    if( HasFix == true )
    {
        SeenSatellites = atoi( NmeaGpsData.NmeaSatelliteTracked );
    }
    else
    {
    	SeenSatellites = 0;
    }
    BoardEnableIrq( );
    return SeenSatellites;
}

double GpsGetHdop(void)
{
    BoardDisableIrq( );
    if( HasFix == true )
    {
        Hdop = atof( NmeaGpsData.NmeaHorizontalDilution );
    }
    else
    {
    	Hdop = 50;
    }
    BoardEnableIrq( );
    return Hdop;
}

double GpsGetAccuracy(void)
{
    BoardDisableIrq( );
    if( HasFix == true )
    {
        AccuracyMtrs = atof( NmeaGpsData.HorizontalAccuracy );
    }
    else
    {
    	AccuracyMtrs = 100;
    }
    BoardEnableIrq( );
    return AccuracyMtrs;
}

/*!
 * Calculates the checksum for a NMEA sentence
 *
 * Skip the first '$' if necessary and calculate checksum until '*' character is
 * reached (or buffSize exceeded).
 *
 * \retval chkPosIdx Position of the checksum in the sentence
 */
int32_t GpsNmeaChecksum( int8_t *nmeaStr, int32_t nmeaStrSize, int8_t * checksum )
{
    int i = 0;
    uint8_t checkNum = 0;

    // Check input parameters
    if( ( nmeaStr == NULL ) || ( checksum == NULL ) || ( nmeaStrSize <= 1 ) )
    {
        return -1;
    }

    // Skip the first '$' if necessary
    if( nmeaStr[i] == '$' )
    {
        i += 1;
    }

    // XOR until '*' or max length is reached
    while( nmeaStr[i] != '*' )
    {
        checkNum ^= nmeaStr[i];
        i += 1;
        if( i >= nmeaStrSize )
        {
            return -1;
        }
    }

    // Convert checksum value to 2 hexadecimal characters
    checksum[0] = Nibble2HexChar( checkNum / 16 ); // upper nibble
    checksum[1] = Nibble2HexChar( checkNum % 16 ); // lower nibble

    return i + 1;
}

/*!
 * Calculate the checksum of a NMEA frame and compare it to the checksum that is
 * present at the end of it.
 * Return true if it matches
 */
static bool GpsNmeaValidateChecksum( int8_t *serialBuff, int32_t buffSize )
{
    int32_t checksumIndex;
    int8_t checksum[2]; // 2 characters to calculate NMEA checksum

    checksumIndex = GpsNmeaChecksum( serialBuff, buffSize, checksum );

    // could we calculate a verification checksum ?
    if( checksumIndex < 0 )
    {
        return false;
    }

    // check if there are enough char in the serial buffer to read checksum
    if( checksumIndex >= ( buffSize - 2 ) )
    {
        return false;
    }

    // check the checksum
    if( ( serialBuff[checksumIndex] == checksum[0] ) && ( serialBuff[checksumIndex + 1] == checksum[1] ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/* Parse Nmea-Field from Buff at BuffOffset, BuffOffset is incremented automatically*/
int8_t GpsParseField(char *Field, uint8_t FieldSize, int8_t *Buffer, uint8_t *BufferOffset)
{
	uint8_t Size = 0;

	while( Buffer[*BufferOffset + Size++] != ',' )
	{
		if( Size > FieldSize )
		{
			return -1;
		}
	}

	uint8_t j;
	for( j = 0; j < Size; j++)
	{
		Field[j] = Buffer[*BufferOffset + j];
	}

	Field[j-1] = '\0';

	*BufferOffset += Size;
	return Size;
}

uint8_t GpsParseGpsData( int8_t *rxBuffer, int32_t rxBufferSize )
{
    uint8_t BufferOffset = 1;
    int8_t Parsed = 0;
    
    if( rxBuffer[0] != '$' )
    {
        GpsMcuInvertPpsTrigger( );
        return FAIL;
    }

    if( GpsNmeaValidateChecksum( rxBuffer, rxBufferSize ) == false )
    {
        return FAIL;
    }

    Parsed = GpsParseField(NmeaGpsData.NmeaDataType, 6, rxBuffer, &BufferOffset);
    if (Parsed < 0)
    {
    	return FAIL;
    }

    // Parse the GPGGA data 
    if( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypeGPGGA, 5 ) == 0 )
    {  
        // NmeaUtcTime
    	Parsed = GpsParseField(NmeaGpsData.NmeaUtcTime, 11, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaLatitude
    	Parsed = GpsParseField(NmeaGpsData.NmeaLatitude, 10, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaLatitudePole
    	Parsed = GpsParseField(NmeaGpsData.NmeaLatitudePole, 2, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaLongitude
    	Parsed = GpsParseField(NmeaGpsData.NmeaLongitude, 11, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaLongitudePole
    	Parsed = GpsParseField(NmeaGpsData.NmeaLongitudePole, 2, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaFixQuality
    	Parsed = GpsParseField(NmeaGpsData.NmeaFixQuality, 2, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaSatelliteTracked
    	Parsed = GpsParseField(NmeaGpsData.NmeaSatelliteTracked, 3, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaHorizontalDilution
    	Parsed = GpsParseField(NmeaGpsData.NmeaHorizontalDilution, 6, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaAltitude
    	Parsed = GpsParseField(NmeaGpsData.NmeaAltitude, 8, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaAltitudeUnit
    	Parsed = GpsParseField(NmeaGpsData.NmeaAltitudeUnit, 2, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaHeightGeoid
    	Parsed = GpsParseField(NmeaGpsData.NmeaHeightGeoid, 8, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaHeightGeoidUnit
    	Parsed = GpsParseField(NmeaGpsData.NmeaHeightGeoidUnit, 2, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

        GpsFormatGpsData( );
        return SUCCESS;
    }
    else if ( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypeGPRMC, 5 ) == 0 )
    {    
    	// NmeaUtcTime
    	Parsed = GpsParseField(NmeaGpsData.NmeaUtcTime, 11, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaDataStatus
    	Parsed = GpsParseField(NmeaGpsData.NmeaDataStatus, 2, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaLatitude
    	Parsed = GpsParseField(NmeaGpsData.NmeaLatitude, 10, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaLatitudePole
    	Parsed = GpsParseField(NmeaGpsData.NmeaLatitudePole, 2, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaLongitude
    	Parsed = GpsParseField(NmeaGpsData.NmeaLongitude, 11, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaLongitudePole
    	Parsed = GpsParseField(NmeaGpsData.NmeaLongitudePole, 2, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaSpeed
    	Parsed = GpsParseField(NmeaGpsData.NmeaSpeed, 8, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaDetectionAngle
    	Parsed = GpsParseField(NmeaGpsData.NmeaDetectionAngle, 8, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

    	// NmeaDate
		Parsed = GpsParseField(NmeaGpsData.NmeaDate, 8, rxBuffer, &BufferOffset);
		if (Parsed < 0)
		{
			return FAIL;
		}

		GpsFormatGpsData( );
		return SUCCESS;
    }
    else if ( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypePUBX, 4 ) == 0 )
    {
    	char DummyBuffer[11];

    	//msgId
    	Parsed = GpsParseField(NmeaGpsData.NmeaDataStatus, 2, rxBuffer, &BufferOffset);
    	if (Parsed < 0)
    	{
    		return FAIL;
    	}

        // NmeaUtcTime
     	Parsed = GpsParseField(NmeaGpsData.NmeaUtcTime, 11, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// NmeaLatitude
     	Parsed = GpsParseField(NmeaGpsData.NmeaLatitude, 10, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// NmeaLatitudePole
     	Parsed = GpsParseField(NmeaGpsData.NmeaLatitudePole, 2, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// NmeaLongitude
     	Parsed = GpsParseField(NmeaGpsData.NmeaLongitude, 11, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// NmeaLongitudePole
     	Parsed = GpsParseField(NmeaGpsData.NmeaLongitudePole, 2, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// NmeaAltitude
     	Parsed = GpsParseField(NmeaGpsData.NmeaAltitude, 8, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// NmeaFixQuality
     	Parsed = GpsParseField(NmeaGpsData.NmeaFixQuality, 2, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// HorizontalAccuracy
     	Parsed = GpsParseField(NmeaGpsData.HorizontalAccuracy, 6, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// VerticalAccuracy
     	Parsed = GpsParseField(NmeaGpsData.VerticalAccuracy, 6, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// NmeaSpeed
     	Parsed = GpsParseField(NmeaGpsData.NmeaSpeed, 8, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// Dummy for CourseOverGround
     	Parsed = GpsParseField(DummyBuffer, 11, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// Dummy for CourseOverGround
     	Parsed = GpsParseField(DummyBuffer, 11, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}

     	// Dummy for CourseOverGround
     	Parsed = GpsParseField(DummyBuffer, 11, rxBuffer, &BufferOffset);
		if (Parsed < 0)
		{
			return FAIL;
		}

		// NmeaHorizontalDilution
		Parsed = GpsParseField(NmeaGpsData.NmeaHorizontalDilution, 6, rxBuffer, &BufferOffset);
		if (Parsed < 0)
		{
			return FAIL;
		}

		//  Dummy for VerticalDilution
		Parsed = GpsParseField(DummyBuffer, 11, rxBuffer, &BufferOffset);
		if (Parsed < 0)
		{
			return FAIL;
		}

		//  Dummy for TimeDilution
		Parsed = GpsParseField(DummyBuffer, 11, rxBuffer, &BufferOffset);
		if (Parsed < 0)
		{
			return FAIL;
		}

		// NmeaSatelliteTracked
     	Parsed = GpsParseField(NmeaGpsData.NmeaSatelliteTracked, 3, rxBuffer, &BufferOffset);
     	if (Parsed < 0)
     	{
     		return FAIL;
     	}


     	GpsFormatGpsData( );
     	return SUCCESS;
    }
    else
    {
        return FAIL;
    }
}

void GpsFormatGpsData( void )
{
    if( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypeGPGGA, 5 ) == 0 )
    {
        HasFix = ( NmeaGpsData.NmeaFixQuality[0] > 0x30 ) ? true : false;
    }
    else if ( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypeGPRMC, 5 ) == 0 )
    {
        HasFix = ( NmeaGpsData.NmeaDataStatus[0] == 0x41 ) ? true : false;
    }
    else if ( strncmp( ( const char* )NmeaGpsData.NmeaDataType, ( const char* )NmeaDataTypePUBX, 4 ) == 0 )
    {
        HasFix = ( NmeaGpsData.NmeaFixQuality[1] == '3' ) ? true : false; //G3 or D3 Fix?
    }
    GpsConvertPositionFromStringToNumerical( );
    GpsConvertPositionIntoBinary( );
}

void GpsResetPosition( void )
{
    Altitude = 0xFFFF;
    Latitude = 0;
    Longitude = 0;
    LatitudeBinary = 0;
    LongitudeBinary = 0;
    Hdop = 50;
    SeenSatellites = 0;
    AccuracyMtrs = 100;
}
