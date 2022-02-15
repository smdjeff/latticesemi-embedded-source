/**************************************************************
*
* Lattice Semiconductor Corp. Copyright 2008
* 
* ispVME Embedded allows programming of Lattice's suite of FPGA
* devices on embedded systems through the JTAG port.  The software
* is distributed in source code form and is open to re - distribution
* and modification where applicable.
*
*
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vmopcode.h"

#define __VME2HEX__ 1
#ifndef __VME2HEX__
#error __VME2HEX__ must be defined!
#endif

/***************************************************************
*
* File pointer to the VME file.
*
***************************************************************/

FILE * g_pVMEFile = NULL;

/***************************************************************
*
* File pointer to the HEX file.
*
***************************************************************/

FILE * g_pHEXFile = NULL;

/***************************************************************
*
* File pointer to the header file.
*
***************************************************************/

FILE * g_pVMEHeaderFile = NULL;

/***************************************************************
*
* External variables and functions.
*
***************************************************************/

extern signed char ispVMCode();
extern void ispVMCalculateCRC32( unsigned char a_ucData );
extern void error_handler( short a_siRetCode, char * pszMessage );
extern unsigned short g_usCalculatedCRC;
extern unsigned short g_usDataType;
extern unsigned char * g_pucOutMaskData,
                     * g_pucInData,
					 * g_pucOutData,
					 * g_pucHIRData,
					 * g_pucTIRData,
					 * g_pucHDRData,
					 * g_pucTDRData,
					 * g_pucOutDMaskData,
                     * g_pucIntelBuffer;
extern unsigned char * g_pucHeapMemory;
extern unsigned short g_iHeapCounter;
extern unsigned short g_iHEAPSize;
extern unsigned short g_usIntelDataIndex;
extern unsigned short g_usIntelBufferSize;
extern const char * const g_szSupportedVersions[];
extern unsigned short g_usTDOSize;
extern unsigned short g_usMASKSize;
extern unsigned short g_usTDISize;
extern unsigned short g_usDMASKSize;
extern unsigned short g_usLCOUNTSize;
extern unsigned short g_usHDRSize;
extern unsigned short g_usTDRSize;
extern unsigned short g_usHIRSize;
extern unsigned short g_usTIRSize;
extern unsigned short g_usHeapSize;
extern LVDSPair * g_pLVDSList;
extern unsigned short g_usLVDSPairCount;

/***************************************************************
*
* GetByte
*
* Returns a byte to the caller. The returned byte depends on the
* g_usDataType register. If the HEAP_IN bit is set, then the byte
* is returned from the HEAP. If the LHEAP_IN bit is set, then
* the byte is returned from the intelligent buffer. Otherwise,
* the byte is returned directly from the VME file.
*
***************************************************************/

unsigned char GetByte()
{
	unsigned char ucData;
	
	if ( g_usDataType & HEAP_IN ) {

		/***************************************************************
		*
		* Get data from repeat buffer.
		*
		***************************************************************/

		if ( g_iHeapCounter > g_iHEAPSize ) {

			/***************************************************************
			*
			* Data over-run.
			*
			***************************************************************/

			return 0xFF;
		}

		ucData = g_pucHeapMemory[ g_iHeapCounter++ ];
	}
	else if ( g_usDataType & LHEAP_IN ) {

		/***************************************************************
		*
		* Get data from intel buffer.
		*
		***************************************************************/

		if ( g_usIntelDataIndex >= g_usIntelBufferSize ) {

			/***************************************************************
			*
			* Data over-run.
			*
			***************************************************************/

			return 0xFF;
		}

		ucData = g_pucIntelBuffer[ g_usIntelDataIndex++ ];
	}
	else {

		/***************************************************************
		*
		* Get data from file.
		*
		***************************************************************/

		ucData = (unsigned char) fgetc( g_pVMEFile );

		if ( feof( g_pVMEFile ) ) {

			/***************************************************************
			*
			* Reached EOF.
			*
			***************************************************************/

			return 0xFF;
		}
	}
	
	return ( ucData );
}

/***************************************************************
*
* ispVMMemManager
*
* Allocate memory based on cTarget. The memory size is specified
* by usSize.
*
***************************************************************/

void ispVMMemManager( signed char cTarget, unsigned short usSize )
{
	static unsigned short usPreviousSize = 0;
	switch ( cTarget ) {
	case XTDI:
    case TDI:  
		if ( g_pucInData != NULL ) {
			if ( usPreviousSize == usSize ) {/*memory exist*/
				break;
			}
			else {
				free( g_pucInData );
				g_pucInData = NULL;
			}
		}
		g_pucInData = ( unsigned char * ) malloc( usSize / 8 + 2 );
		usPreviousSize = usSize;
    case XTDO:
    case TDO:
		if ( g_pucOutData!= NULL ) { 
			if ( usPreviousSize == usSize ) { /*already exist*/
				break;
			}
			else {
				free( g_pucOutData );
				g_pucOutData = NULL;
			}
		}
		g_pucOutData = ( unsigned char * ) malloc( usSize / 8 + 2 );
		usPreviousSize = usSize;
		break;
    case MASK:
		if ( g_pucOutMaskData != NULL ) {
			if ( usPreviousSize == usSize ) {/*already allocated*/
				break;
			}
			else {
				free( g_pucOutMaskData ); 
				g_pucOutMaskData = NULL;
			}
		}
		g_pucOutMaskData = ( unsigned char * ) malloc( usSize / 8 + 2 );
		usPreviousSize = usSize;
		break;
    case HIR:
		if ( g_pucHIRData != NULL ) {
			free( g_pucHIRData );
			g_pucHIRData = NULL;
		}
		g_pucHIRData = ( unsigned char * ) malloc( usSize / 8 + 2 );
		break;
    case TIR:
		if ( g_pucTIRData != NULL ) {
			free( g_pucTIRData );
			g_pucTIRData = NULL;
		}
		g_pucTIRData = ( unsigned char * ) malloc( usSize / 8 + 2 );
		break;
    case HDR:
		if ( g_pucHDRData != NULL ) {
			free( g_pucHDRData );
			g_pucHDRData = NULL;
		}
		g_pucHDRData = ( unsigned char * ) malloc( usSize / 8 + 2 );
		break;
    case TDR:
		if ( g_pucTDRData != NULL ) {
			free( g_pucTDRData );
			g_pucTDRData = NULL;
		}
		g_pucTDRData = ( unsigned char * ) malloc( usSize / 8 + 2 );
		break;
    case HEAP:
		if ( g_pucHeapMemory != NULL ) {
			free( g_pucHeapMemory );
			g_pucHeapMemory = NULL;
		}
		g_pucHeapMemory = ( unsigned char * ) malloc( usSize + 2 );
		break;
	case DMASK: 
		if ( g_pucOutDMaskData != NULL ) {
			if ( usPreviousSize == usSize ) { /*already allocated*/
				break;
			}
			else {
				free( g_pucOutDMaskData ); 
				g_pucOutDMaskData = NULL;
			}
		}
		g_pucOutDMaskData = ( unsigned char * ) malloc( usSize / 8 + 2 );
		usPreviousSize = usSize;
		break;
	case LHEAP:
		if ( g_pucIntelBuffer != NULL ) {
			free( g_pucIntelBuffer );
			g_pucIntelBuffer = NULL;
		}
		g_pucIntelBuffer = ( unsigned char * ) malloc( usSize + 2 );
		break;
	case LVDS:
		if ( g_pLVDSList != NULL ) {
			free( g_pLVDSList );
			g_pLVDSList = NULL;
		}
		g_pLVDSList = ( LVDSPair * ) calloc( usSize, sizeof( LVDSPair ) );
		break;
	default:
		return;
    }
}

/***************************************************************
*
* ispVMFreeMem
*
* Free memory that were dynamically allocated.
*
***************************************************************/

void ispVMFreeMem()
{
	if ( g_pucHeapMemory != NULL ) {
		free( g_pucHeapMemory ); 
		g_pucHeapMemory = NULL;
	}

	if ( g_pucOutMaskData != NULL ) {
		free( g_pucOutMaskData );
		g_pucOutMaskData = NULL;
	}
	
	if ( g_pucInData != NULL ) {
		free( g_pucInData );
		g_pucInData = NULL;
	}
	
	if ( g_pucOutData != NULL ) {
		free( g_pucOutData );
		g_pucOutData = NULL;
	}
	
	if ( g_pucHIRData != NULL ) {
		free( g_pucHIRData );
		g_pucHIRData = NULL;
	}
	
	if ( g_pucTIRData != NULL ) {
		free( g_pucTIRData );
		g_pucTIRData = NULL;
	}
	
	if ( g_pucHDRData != NULL ) {
		free( g_pucHDRData );
		g_pucHDRData = NULL;
	}
	
	if ( g_pucTDRData != NULL ) {
		free( g_pucTDRData );
		g_pucTDRData = NULL;
	}
	
	if ( g_pucOutDMaskData != NULL ) {
		free( g_pucOutDMaskData );
		g_pucOutDMaskData = NULL;
	}
	
	if ( g_pucIntelBuffer != NULL ) {
		free( g_pucIntelBuffer );
		g_pucIntelBuffer = NULL;
	}

	if ( g_pLVDSList != NULL ) {
		free( g_pLVDSList );
		g_pLVDSList = NULL;
	}
} 

/***************************************************************
*
* ispVM
*
* The entry point of the ispVM embedded. If the version and CRC
* are verified, then the VME will be processed.
*
***************************************************************/

signed char ispVM( const char * a_pszFilename )
{
	char szFileVersion[ 9 ] = { 0 };
	signed char cRetCode = 0;
	signed char cIndex;
	signed char cVersionIndex;
	unsigned char ucReadByte;
	unsigned short usExpectedCRC = 0;
	int iCRCData;
	
	/***************************************************************
	*
	* Open a file pointer to the VME file.
	*
	***************************************************************/

	if ( ( g_pVMEFile = fopen( a_pszFilename, "rb" ) ) == NULL ) {
		return VME_FILE_READ_FAILURE;
	}

	g_usCalculatedCRC = 0;
	ucReadByte = GetByte();
	switch( ucReadByte ) {
	case FILE_CRC:

		/***************************************************************
		*
		* Get the expected CRC.  Only versions 3.0 and higher support
		* CRC protection.
		*
		***************************************************************/

		usExpectedCRC = (unsigned char) fgetc( g_pVMEFile );
		usExpectedCRC <<= 8;
		usExpectedCRC |= fgetc( g_pVMEFile );

		while ( ( iCRCData = getc( g_pVMEFile ) ) != EOF ) {
			ispVMCalculateCRC32( ( unsigned char ) iCRCData );
		}

		/***************************************************************
		*
		* Compare the expected CRC versus the calculated CRC.
		*
		***************************************************************/

		if ( usExpectedCRC && ( usExpectedCRC != g_usCalculatedCRC ) ) {
			printf( "Expected CRC:   0x%.4X\n", usExpectedCRC );
			printf( "Calculated CRC: 0x%.4X\n", g_usCalculatedCRC );
			fclose( g_pVMEFile );
			g_pVMEFile = 0;
			return VME_CRC_FAILURE;
		}

		/***************************************************************
		*
		* Rewind the VME file pointer back to the beginning and discard
		* the three CRC related bytes.
		*
		***************************************************************/

		rewind( g_pVMEFile );
		GetByte();
		GetByte();
		GetByte();

		/***************************************************************
		*
		* Read and store the version of the VME file.
		*
		***************************************************************/

		for ( cIndex = 0; cIndex < 8; cIndex++ ) {
			szFileVersion[ cIndex ] = GetByte();
		}

		break;
	default:

		/***************************************************************
		*
		* Read and store the version of the VME file.  Must be version 2.0.
		*
		***************************************************************/

		szFileVersion[ 0 ] = ( signed char ) ucReadByte;
		for ( cIndex = 1; cIndex < 8; cIndex++ ) {
			szFileVersion[ cIndex ] = GetByte();
		}

		break;
	}

	/***************************************************************
	*
	* Compare the VME file version against the supported version.
	*
	***************************************************************/

	for ( cVersionIndex = 0; g_szSupportedVersions[ cVersionIndex ] != 0; cVersionIndex++ ) {
		for ( cIndex = 0; cIndex < 8; cIndex++ ) {
			if ( szFileVersion[ cIndex ] != g_szSupportedVersions[ cVersionIndex ][ cIndex ] ) {
				cRetCode = VME_VERSION_FAILURE;
				break;
			}	
			cRetCode = 0;
		}

		if ( cRetCode == 0 ) {

			/***************************************************************
			*
			* Found matching version, break.
			*
			***************************************************************/

			break;
		}
	}

	if ( cRetCode < 0 ) {

		/***************************************************************
		*
		* VME file version failed to match the supported versions.
		*
		***************************************************************/

		fclose( g_pVMEFile );
		return VME_VERSION_FAILURE;
	}

	/***************************************************************
	*
	* Process the VME file.
	*
	***************************************************************/

    cRetCode = ispVMCode();
                   
    fclose( g_pVMEFile );
	g_pVMEFile = 0;
    return ( cRetCode );
}

/***************************************************************
*
* main
*
***************************************************************/

int main( int argc, char * argv[] )
{
	short siRetCode = 0;
	char szCommandLineArg[ 300 ] = { 0 };
	int iVMESize = 0;
	short int iHexFileCount = 0;
	short int iHexFileCountIndex = 0;
	long * pHexFileSizes = 0;
	long int iHexFileSize = VMEHEXMAX;
	long int iHexFileSizeIndex;
	char szOutputFilename[ 1024 ] = { 0 };
	char szOutputHeaderFilename[ 1024 ] = { 0 };
	char szOutputDirectory[ 1024 ] = { 0 };
	char szOutputExtension[ 20 ] = { 0 };
	char * pOutputFileDirectory = NULL;
	
	printf("\n                 Lattice Semiconductor Corp.\n");
	printf("\nispVM Embedded File to HEX String Converter V%s Copyright 2000-2011\n\n", VME_VERSION_NUMBER);
	
	if ( argc != 3 ) {
		printf( "Usage: vme2hex < vme_file > < hex_file >\n\n" );
		return VME_ARGUMENT_FAILURE;
	}

	/***************************************************************
	*
	* Establish the output directory name.
	*
	***************************************************************/

	strcpy( szOutputDirectory, argv[ 2 ] );
	if ( strrchr( szOutputDirectory, '\\' ) > strrchr( szOutputDirectory, '/' ) ) {
		pOutputFileDirectory = strrchr( szOutputDirectory, '\\' );
		pOutputFileDirectory++;
		*pOutputFileDirectory = '\0';
	}
	else if ( strrchr( szOutputDirectory, '\\' ) < strrchr( szOutputDirectory, '/' ) ) {
		pOutputFileDirectory = strrchr( szOutputDirectory, '/' );
		pOutputFileDirectory++;
		*pOutputFileDirectory = '\0';
	}

	/***************************************************************
	*
	* Enter the embedded programming engine to extract the maximum
	* sizes.
	*
	***************************************************************/

	strcpy( szCommandLineArg, argv[ 1 ] );
	printf( "Converting file (%s)......\n\n", szCommandLineArg );
	siRetCode = ispVM( szCommandLineArg );
	ispVMFreeMem();
	if ( siRetCode < 0 ) {
		error_handler( siRetCode, szCommandLineArg );
		printf( "Failed due to %s\n\n", szCommandLineArg );
		printf( "+=======+\n" );
		printf( "| FAIL! |\n" );
		printf( "+=======+\n\n" );
		exit( siRetCode );
	}

	/***************************************************************
	*
	* Write file "vme_file.h".
	*
	***************************************************************/

	sprintf( szOutputHeaderFilename, "%svme_file.h", szOutputDirectory );
	g_pVMEHeaderFile = fopen( szOutputHeaderFilename, "w" );
	if ( !g_pVMEHeaderFile ) {
		printf( "Failed to write file '%s'\n", szOutputHeaderFilename );
		exit( VME_INVALID_FILE );
	}

	fprintf( g_pVMEHeaderFile, "\n/*ispVM Embedded File for ispVM*/" );

	if ( g_usMASKSize ) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char MaskBuf[%d];		/* Memory to hold MASK data. */\n", g_usMASKSize / 8 + 1 );
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nunsigned char *MaskBuf=NULL;		/* Memory to hold MASK data. */\n");
	}

	if ( g_usTDISize ) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char TdiBuf[%d];			/* Memory to hold TDI data. */\n", g_usTDISize / 8 + 1 );
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nunsigned char *TdiBuf=NULL;		/* Memory to hold TDI data. */\n");
	}

	if ( g_usTDOSize) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char TdoBuf[%d];			/* Memory to hold TDO data. */\n", g_usTDOSize / 8 + 1 );
	}
	else {
		if ( g_usDMASKSize ) {

			/***************************************************************
			*
			* If there is no TDO in the VME but DMASK exists, then create
			* a TDO buffer anyway because DMASK implicitly requires TDO.
			*
			***************************************************************/

			fprintf( g_pVMEHeaderFile, "\nunsigned char TdoBuf[%d];			/* Memory to hold TDO data. */\n", g_usDMASKSize / 8 + 1 );
		}
		else {
			fprintf( g_pVMEHeaderFile, "\nunsigned char *TdoBuf=NULL;		/* Memory to hold TDO data. */\n");
		}
	}

	if ( g_usHDRSize ) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char HdrBuf[%d];			/* Memory to hold HDR data. */\n", g_usHDRSize / 8 + 1 );
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nunsigned char *HdrBuf=NULL;		/* Memory to hold HDR data. */\n");
	}

	if ( g_usTDRSize ) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char TdrBuf[%d];			/* Memory to hold TDR data. */\n", g_usTDRSize / 8 + 1 );
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nunsigned char *TdrBuf=NULL;		/* Memory to hold TDR data. */\n");
	}

	if ( g_usHIRSize ) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char HirBuf[%d];			/* Memory to hold HIR data. */\n", g_usHIRSize / 8 + 1 );
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nunsigned char *HirBuf=NULL;		/* Memory to hold HIR data. */\n");
	}

	if ( g_usTIRSize ) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char TirBuf[%d];			/* Memory to hold TIR data. */\n", g_usTIRSize / 8 + 1 );
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nunsigned char *TirBuf=NULL;		/* Memory to hold TIR data. */\n");
	}

	if ( g_usHeapSize ) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char HeapBuf[%d];		/* Memory to hold HEAP data. */\n", g_usHeapSize);
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nunsigned char *HeapBuf=NULL;		/* Memory to hold HEAP data. */\n");
	}

	if ( g_usDMASKSize ) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char DMASKBuf[%d];		/* Memory to hold DMASK data. */\n", g_usDMASKSize / 8 + 1 );
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nunsigned char *DMASKBuf=NULL;		/* Memory to hold DMASK data. */\n");
	}
	
	if ( g_usLCOUNTSize ) {
		fprintf( g_pVMEHeaderFile, "\nunsigned char LCOUNTBuf[ %d ];	/* Memory to hold LCOUNT data. */\n", g_usLCOUNTSize + 1 );
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nunsigned char * LCOUNTBuf = NULL;	/* Memory to hold LCOUNT data. */\n" );
	}

	if ( g_usLVDSPairCount ) {
		fprintf( g_pVMEHeaderFile, "\nLVDSPair LVDSBuf[ %d ];	/* Memory to hold LVDS pairs. */\n", g_usLVDSPairCount );
	}
	else {
		fprintf( g_pVMEHeaderFile, "\nLVDSPair * LVDSBuf = NULL;	/* Memory to hold LVDS Pair. */\n" );
	}

	/***************************************************************
	*
	* Count the number of bytes in the VME file.
	*
	***************************************************************/

	g_pVMEFile = fopen( argv[ 1 ], "rb" );
	while ( fgetc( g_pVMEFile ) != EOF ) {
		iVMESize++;
	}
	rewind( g_pVMEFile );

	/***************************************************************
	*
	* Calculate the number of hex files that will be needed.
	*
	***************************************************************/

	if ( iVMESize % VMEHEXMAX ) {
		iHexFileCount = ( short int ) ( iVMESize / VMEHEXMAX + 1 );
	}
	else {
		iHexFileCount = ( short int ) ( iVMESize / VMEHEXMAX );
	}

	/***************************************************************
	*
	* Remove the extension from the output file name.
	*
	***************************************************************/

	if ( strrchr( argv[ 2 ], '.' ) ) {
		strcpy( szOutputExtension, strrchr( argv[ 2 ], '.' ) );
		*( strrchr( argv[ 2 ], '.' ) ) = '\0';
	}

	/***************************************************************
	*
	* Iterate through each hex file and write it.
	*
	***************************************************************/

	pHexFileSizes = ( long * ) calloc( iHexFileCount + 1, sizeof( long ) );
	for ( iHexFileCountIndex = 1; iHexFileCountIndex <= iHexFileCount; iHexFileCountIndex++ ) {
		if ( ( iVMESize - VMEHEXMAX ) > 0 ) {
			iHexFileSize = VMEHEXMAX;
			iVMESize -= VMEHEXMAX;
		}
		else {
			iHexFileSize = iVMESize;
		}

		pHexFileSizes[ iHexFileCountIndex - 1 ] = iHexFileSize;
		sprintf( szOutputFilename, "%s%d%s", argv[ 2 ], iHexFileCountIndex, szOutputExtension );

		/***************************************************************
		*
		* Open file pointer to individual hex file.
		*
		***************************************************************/

		if ( ( g_pHEXFile = fopen( szOutputFilename, "w" ) ) == NULL ) {
			printf( "Failed to write file \'%s\'\n", szOutputFilename );
			exit( VME_INVALID_FILE );
		}

		fprintf( g_pVMEHeaderFile, "\nextern unsigned char vme%d[%ldL];\n", iHexFileCountIndex, iHexFileSize );
		fprintf( g_pHEXFile, "\nunsigned char vme%d[%ldL] = {\n", iHexFileCountIndex, iHexFileSize );

		/***************************************************************
		*
		* Write iHexFileSize number of bytes to the file.
		*
		***************************************************************/

		for ( iHexFileSizeIndex = 1; iHexFileSizeIndex < iHexFileSize; iHexFileSizeIndex++ ) {
			fprintf( g_pHEXFile, "0x%02X, ", fgetc( g_pVMEFile ) );
			if ( iHexFileSizeIndex % 15 == 0 ) {
				fprintf( g_pHEXFile, "\n" );
			}
		}

		/***************************************************************
		*
		* Close file pointer to individual hex file.
		*
		***************************************************************/

		fprintf( g_pHEXFile, "0x%02X", fgetc( g_pVMEFile ) );
		fprintf( g_pHEXFile, "};\n" );
		fclose( g_pHEXFile );
	}
	
	fprintf( g_pVMEHeaderFile, "\nshort int     vmearrays = %d;\n",iHexFileCount );
	fprintf( g_pVMEHeaderFile, "unsigned char *vmeArray[%d] = {", iHexFileCount );
	for ( iHexFileCountIndex = 1; iHexFileCountIndex <= iHexFileCount; iHexFileCountIndex++ ) {
		if ( iHexFileCountIndex == iHexFileCount ) {
			fprintf( g_pVMEHeaderFile, "vme%d", iHexFileCountIndex );
		}
		else {
			fprintf( g_pVMEHeaderFile, "vme%d, ", iHexFileCountIndex );
		}
	}
	fprintf( g_pVMEHeaderFile, "};\n" );

	fprintf( g_pVMEHeaderFile, "long g_pArraySizes[ %d ] = { ", iHexFileCount );
	for( iHexFileCountIndex = 0; iHexFileCountIndex < iHexFileCount; iHexFileCountIndex++ ) {
		if ( iHexFileCountIndex != iHexFileCount - 1 ) {
			fprintf( g_pVMEHeaderFile, "%dL, ", pHexFileSizes[ iHexFileCountIndex ] );
		}
		else {
			fprintf( g_pVMEHeaderFile, "%dL };", pHexFileSizes[ iHexFileCountIndex ] );
		}
	}

	/***************************************************************
	*
	* Free memory and close file pointers.
	*
	***************************************************************/

	free( pHexFileSizes );
	pHexFileSizes = 0;
	fclose( g_pVMEHeaderFile );
	fclose( g_pVMEFile );

	/***************************************************************
	*
	* Print the generated files.
	*
	***************************************************************/

	printf( "\nispVM Embedded Files Generated:\n\n" );
	for( iHexFileCountIndex = 1; iHexFileCountIndex <= iHexFileCount; iHexFileCountIndex++ ) {
		printf( "\t-%s%d%s\n", argv[ 2 ], iHexFileCountIndex, szOutputExtension );
	}
	printf( "\t-%s\n\n", szOutputHeaderFilename );

	exit( 0 );
} 

