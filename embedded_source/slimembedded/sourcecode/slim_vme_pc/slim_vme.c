/**************************************************************
*
* Lattice Semiconductor Corp. Copyright 2008
* 
*
***************************************************************/


/**************************************************************
* 
* Revision History of slim_vme.c
* 
* 
* 09/11/07 NN Updated to support version 1.3
* This version supported new POLING STATUS LOOP opcodes (LOOP and ENDLOOP) 
* for Flash programming of the Lattice FPGA devices
* 09/11/07 NN Added Global variables initialization
* 09/11/07 NN type cast all mismatch variables
***************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include "opcode.h"

#ifdef VME_WINDOWS
#include <windows.h>
#include "windriver.h"
#include <time.h>
#endif /* VME_WINDOWS */

/*************************************************************
*                                                            *
* EXTERNAL FUNCTIONS                                         *
*                                                            *
*************************************************************/

extern unsigned char GetByte( int a_iCurrentIndex, char a_cAlgo );
extern short ispProcessVME();
extern void EnableHardware();
extern void DisableHardware();

#ifdef VME_WINDOWS
extern int Check_Cable_Power();
#endif

/***************************************************************
*
* Supported VME versions.
*
* 09/11/07 NN Updated to support version 1.3
***************************************************************/

const char * const g_szSupportedVersions[] = { "_SVME1.1", "_SVME1.2", "_SVME1.3", "_SVME1.4", 0 };

/*************************************************************
*                                                            *
* GLOBAL VARIABLES                                           *
*                                                            *
*************************************************************/

unsigned char * g_pucAlgoArray = NULL;	/*** array to hold the algorithm ***/
unsigned char * g_pucDataArray = NULL;	/*** array to hold the data ***/
int g_iAlgoSize = 0;					/*** variable to hold the size of the algorithm array ***/
int g_iDataSize = 0;					/*** variable to hold the size of the data array ***/

/*************************************************************
*                                                            *
* EXTERNAL VARIABLES                                         *
*                                                            *
*************************************************************/

extern unsigned short g_usDataType;
extern int g_iMovingAlgoIndex;
extern int g_iMovingDataIndex;
extern unsigned char g_iCompressKey;

/*************************************************************
*                                                            *
* ISPVMENTRYPOINT                                            *
*                                                            *
* INPUT:                                                     *
*     a_pszAlgoFile: this is the name of the algorithm file. *
*                                                            *
*     a_pszDataFile: this is the name of the data file.      *
*     Note that this argument may be empty if the algorithm  *
*     does not require a data file.                          *
*                                                            *
* RETURN:                                                    *
*     The return value will be a negative number if an error *
*     occurred, or 0 if everything was successful            *
*                                                            *
* DESCRIPTION:                                               *
*     This function opens the file pointers to the algo and  *
*     data file.  It intializes global variables to their    *
*     default values and enters the processor.               *
*                                                            *
*************************************************************/

short int ispEntryPoint( const char * a_pszAlgoFile, const char * a_pszDataFile )
{
	char szFileVersion[ 9 ] = { 0 };
	short int siRetCode     = 0;
	int iIndex              = 0;
	int cVersionIndex       = 0;
	FILE * pFile            = NULL;
	unsigned char byteData = 0;
	
	/*************************************************************
	*                                                            *
	* VARIABLES INITIALIZATION                                         *
	*                                                            *
	*************************************************************/

	g_pucAlgoArray     = NULL;	
	g_pucDataArray     = NULL;	
	g_iAlgoSize        = 0;
	g_iDataSize        = 0;
	g_usDataType       = 0;
	g_iMovingAlgoIndex = 0;
	g_iMovingDataIndex = 0;

	/*************************************************************
	*                                                            *
	* Open the algorithm file, get the size in bytes, allocate   *
	* memory, and read it in.                                    *
	*                                                            *
	*************************************************************/

	if ( ( pFile = fopen( a_pszAlgoFile, "rb" ) ) == NULL ) {
		return ERR_FIND_ALGO_FILE;
	}

	for ( g_iAlgoSize = 0; !feof( pFile ); g_iAlgoSize++ ) {
		getc( pFile );
	}
	g_iAlgoSize--;

	g_pucAlgoArray = ( unsigned char * ) malloc( g_iAlgoSize + 1 );
	if ( !g_pucAlgoArray ) {
		fclose( pFile );
		return ERR_OUT_OF_MEMORY;
	}

	rewind( pFile );
	for ( iIndex = 0; !feof( pFile ); ++iIndex ) {
		g_pucAlgoArray[ iIndex ] = (unsigned char) getc( pFile );
	}
	fclose( pFile );
	
	/*************************************************************
	*                                                            *
	* Open the data file, get the size in bytes, allocate        *
	* memory, and read it in.                                    *
	*                                                            *
	*************************************************************/

	if ( a_pszDataFile ) {
		if ( ( pFile = fopen( a_pszDataFile, "rb" ) ) == NULL ) {
			free( g_pucAlgoArray );
			return ERR_FIND_DATA_FILE;
		}

		for ( g_iDataSize = 0; !feof( pFile ); g_iDataSize++ ) {
			getc( pFile );
		}
		g_iDataSize--;

		g_pucDataArray = ( unsigned char * ) malloc( g_iDataSize + 1 );
		if ( !g_pucDataArray ) {
			free( g_pucAlgoArray );
			fclose( pFile );
			return ERR_OUT_OF_MEMORY;
		}

		rewind( pFile );
		for ( iIndex = 0; !feof( pFile ); ++iIndex ) {
			g_pucDataArray[ iIndex ] = (unsigned char) getc( pFile );
		}
		fclose( pFile );

		byteData = GetByte( g_iMovingDataIndex++, 0 );
		if ( byteData == 0x01 ) {
			g_usDataType |= COMPRESS;
			g_iCompressKey = 0xFF;
		}
		else if ( byteData == 0x02 ) {
			g_usDataType |= COMPRESS;
			g_iCompressKey = 0x00;
		}
	}

	/***************************************************************
	*
	* Read and store the version of the VME file.
	*
	***************************************************************/

	for ( iIndex = 0; iIndex < 8; iIndex++ ) {
		szFileVersion[ iIndex ] = GetByte( g_iMovingAlgoIndex++, 1 );
	}

	/***************************************************************
	*
	* Compare the VME file version against the supported version.
	*
	***************************************************************/

	for ( cVersionIndex = 0; g_szSupportedVersions[ cVersionIndex ] != 0; cVersionIndex++ ) {
		for ( iIndex = 0; iIndex < 8; iIndex++ ) {
			if ( szFileVersion[ iIndex ] != g_szSupportedVersions[ cVersionIndex ][ iIndex ] ) {
				siRetCode = ERR_WRONG_VERSION;
				break;
			}	
			siRetCode = 0;
		}

		if ( siRetCode == 0 ) {

			/***************************************************************
			*
			* Found matching version, break.
			*
			***************************************************************/

			break;
		}
	}

	if ( siRetCode < 0 ) {

		/***************************************************************
		*
		* VME file version failed to match the supported versions.
		*
		***************************************************************/

		free( g_pucAlgoArray );
		if ( g_pucDataArray ) {
			free( g_pucDataArray );
		}
		g_pucAlgoArray = NULL;
		g_pucDataArray = NULL;
		return ERR_WRONG_VERSION;
	}


	/*************************************************************
	*                                                            *
	* Start the hardware.                                        *
	*                                                            *
	*************************************************************/

    EnableHardware();
                         
	/*************************************************************
	*                                                            *
	* Begin processing algorithm and data file.                  *
	*                                                            *
	*************************************************************/

	siRetCode = ispProcessVME();

	/*************************************************************
	*                                                            *
	* Stop the hardware.                                         *
	*                                                            *
	*************************************************************/

    DisableHardware();

	/*************************************************************
	*                                                            *
	* Free dynamic memory and return value.                      *
	*                                                            *
	*************************************************************/

	free( g_pucAlgoArray );
	if ( g_pucDataArray ) {
		free( g_pucDataArray );
	}
	g_pucAlgoArray = NULL;
	g_pucDataArray = NULL;

    return ( siRetCode );
}

/*************************************************************
*                                                            *
* ERROR_HANDLER                                              *
*                                                            *
* INPUT:                                                     *
*     a_siRetCode: this is the error code reported by the    *
*     processor.                                             *
*                                                            *
*     a_pszMessage: this will store the return message.      *
*                                                            *
* RETURN:                                                    *
*     None.                                                  *
*                                                            *
* DESCRIPTION:                                               *
*     This function assigns an error message based on the    *
*     error reported by the processor.  The program should   *
*     display the error message prior to exiting.            *
*                                                            *
*************************************************************/

#ifdef VME_WINDOWS
void error_handler( short int a_siRetCode, char * a_pszMessage )
{
	switch( a_siRetCode ) {
	case ERR_VERIFY_FAIL:
		strcpy( a_pszMessage, "VERIFY FAIL" );
		break;
	case ERR_FIND_ALGO_FILE:
		strcpy( a_pszMessage, "CANNOT FIND ALGO FILE" );
		break;
	case ERR_FIND_DATA_FILE:
		strcpy( a_pszMessage, "CANNOT FIND DATA FILE" );
		break;
	case ERR_WRONG_VERSION:
		strcpy( a_pszMessage, "WRONG FILE TYPE/VERSION" );
		break;
	case ERR_ALGO_FILE_ERROR:
		strcpy( a_pszMessage, "ALGO FILE ERROR" );
		break;
	case ERR_DATA_FILE_ERROR:
		strcpy( a_pszMessage, "DATA FILE ERROR" );
		break;
	case ERR_OUT_OF_MEMORY:
		strcpy( a_pszMessage, "OUT OF MEMORY" );
		break;
	default:
		strcpy( a_pszMessage, "UNKNOWN ERROR" );
		break;
	}
} 
#endif /* VME_WINDOWS */

/*************************************************************
*                                                            *
* MAIN                                                       *
*                                                            *
*************************************************************/

short int main( int argc, char * argv[] )
{
	/*************************************************************
	*                                                            *
	* LOCAL VARIABLES:                                           *
	*     siRetCode: this variable holds the return.             *
	*                                                            *
	*************************************************************/

	short int siRetCode = 0; 

#ifdef VME_WINDOWS
	char szErrorMessage[ 50 ] = { 0 };
	time_t startTime;
	time_t endTime;
	short int siTime = 0;

	printf( "                 Lattice Semiconductor Corp.\n" );
	printf( "             Slim ispVME(tm) 1.3 Copyright 2011.\n\n" );

	siRetCode = (short int) Check_Cable_Power();
	if ( siRetCode == CABLE_NOT_DETECT ) {
		printf( "Error: download cable not detected\n\n" );
		exit( 1 );
	}
	if ( siRetCode == POWER_OFF ) {
		printf( "Error: the power is not on\n\n" );
		exit( 1 );
	}
#endif /* VME_WINDOWS */

	if ( ( argc < 2 ) || ( argc > 3 ) ) {

#ifdef VME_WINDOWS
		printf( "Usage: vme vme_algo [ vme_data ]\n\n" );
#endif /* VME_WINDOWS */

		exit( -1 );
	}
	
#ifdef VME_WINDOWS
	if ( argc == 2 ) {
		printf( "Processing algorithm file (%s)......\n", argv[ 1 ] );
	}
	else if ( argc == 3 ) {
		printf( "Processing algorithm file (%s)......\n", argv[ 1 ] );
		printf( "\nProcessing data file (%s)......\n", argv[ 2 ] );
	}

	time( &startTime );
#endif /* VME_WINDOWS */

	/*************************************************************
	*                                                            *
	* Pass in the command line arguments to the entry point.     *
	*                                                            *
	*************************************************************/

	siRetCode = ispEntryPoint( argv[ 1 ], argv[ 2 ] );

#ifdef VME_WINDOWS
	time( &endTime );

	/*************************************************************
	*                                                            *
	* Check the return code and report appropriate message       *
	*                                                            *
	*************************************************************/

	if ( siRetCode < 0 ) {
		error_handler( siRetCode, szErrorMessage );
        printf( "\nProcessing failure: %s\n\n", szErrorMessage );
        printf( "+=======+\n" );
        printf( "| FAIL! |\n" );
        printf( "+=======+\n\n" );
	} 
	else {
		printf( "\n+=======+\n" );
        printf( "| PASS! |\n" );
        printf( "+=======+\n\n" );
		siTime = (short int) (endTime - startTime);
		printf( "Processing time: %d\n\n", siTime );
	}
#endif /* VME_WINDOWS */
	
	exit( siRetCode );
} 