/**************************************************************
*
* Lattice Semiconductor Corp. Copyright 2011
* 
*
***************************************************************/


/**************************************************************
* 
* Revision History of i2c_eprom.c
* 
*   Fixed version array index error on 06/13/2013. Added this note on 01/13/2014.
*
* Support version 1.0
***************************************************************/
#include <stdlib.h>
#include "opcode.h"

/***************************************************************
*
* Supported I2C versions.
*
***************************************************************/

const char * const g_szSupportedVersions[] = { "_I2C1.0", 0 };

/*************************************************************
*                                                            *
* EXTERNAL FUNCTIONS                                         *
*                                                            *
*************************************************************/
extern unsigned char GetByte( int a_iCurrentIndex, char a_cAlgo );
extern short ispProcessI2C();
extern void EnableHardware();
extern void DisableHardware();

/*************************************************************
*                                                            *
* EXTERNAL VARIABLES                                         *
*                                                            *
*     If the algorithm does not require the data, then       *
*     declare the variables g_pucDataArray and g_iDataSize   *
*     as local variables and set them to NULL and 0,         *
*     respectively.                                          *
*                                                            *
*     Example:                                               *
*          xdata unsigned char * g_pucDataArray = NULL;      *
*          xdata int g_iDataSize = 0;                        *
*                                                            *
*************************************************************/

extern int g_iMovingAlgoIndex;	    
extern int g_iMovingDataIndex;		
extern unsigned short g_usDataType;
extern xdata const int g_iDataSize;

/*************************************************************
*                                                            *
* ISPI2CNTRYPOINT                                            *
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

short int ispEntryPoint()
{
	char szFileVersion[ 8 ] = { 0 };
	short int siRetCode     = 0;
	short int iIndex        = 0;
	short int cVersionIndex = 0;

	/*************************************************************
	*                                                            *
	* VARIABLES INITIALIZATION                                   *
	*                                                            *
	*************************************************************/

	g_usDataType       = 0;
	g_iMovingAlgoIndex = 0;
	g_iMovingDataIndex = 0;

	if ( g_iDataSize ) {
		if ( GetByte( g_iMovingDataIndex++, 0 ) ) {
			g_usDataType |= COMPRESS;
		}
	}
	/***************************************************************
	*
	* Read and store the version of the VME file.
	*
	***************************************************************/

	for ( iIndex = 0; iIndex < 7; iIndex++ ) {
		szFileVersion[ iIndex ] = GetByte( g_iMovingAlgoIndex++, 1 );
	}

	/***************************************************************
	*
	* Compare the VME file version against the supported version.
	*
	***************************************************************/

	for ( cVersionIndex = 0; g_szSupportedVersions[ cVersionIndex ] != 0; cVersionIndex++ ) {
		for ( iIndex = 0; iIndex < 7; iIndex++ ) {
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

	siRetCode = ispProcessI2C();

	/*************************************************************
	*                                                            *
	* Stop the hardware.                                         *
	*                                                            *
	*************************************************************/

    DisableHardware();

	/*************************************************************
	*                                                            *
	* Return the return code.                                    *
	*                                                            *
	*************************************************************/

	return ( siRetCode );
}
/*************************************************************
*                                                            *
* MAIN                                                       *
*                                                            *
*************************************************************/

short int main()
{
		/*************************************************************
	*                                                            *
	* LOCAL VARIABLES:                                           *
	*     siRetCode: this variable holds the return.             *
	*                                                            *
	*************************************************************/

	short int siRetCode = 0; 
	/*************************************************************
	*                                                            *
	* Pass in the command line arguments to the entry point.     *
	*                                                            *
	*************************************************************/

	siRetCode = ispEntryPoint();	
	return( siRetCode );
} 
