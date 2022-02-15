/**************************************************************
*
* Lattice Semiconductor Corp.
* 
* CPU Embedded allows programming of Lattice's suite of FPGA
* devices on embedded systems through the sysCONFIG port.  The software
* is distributed in source code form and is open to re - distribution
* and modification where applicable.
*
* This is the function - based main for the CPU Embedded.  This
* version allows access to the cpu engine through a function
* interface.  This function is called lattice_main() and takes 
* a pointer to the data array and the size of the data array as
* argument.  This approach allows wrapper applications to call
* lattice_main and pass in different data arrays for different
* devices, hence the moniker "function-based".
*
**************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "cpu_code.h"
#include "cpu_head.h"


/**************************************************************
*
* Local variable declarations.
*
**************************************************************/

unsigned long g_ulDataSize;
const unsigned char * g_pucDataArray;
unsigned long g_ulCurrentByteIndex = 0;
unsigned long storedCurrentByte = 0;

unsigned short g_usCalculatedCRC = 0x0000;


signed char lattice_main(const unsigned char * a_pucDataArray, unsigned long a_ulDataSize)
{
	signed char cRetCode = CPU_SUCCESS;
	
	/**************************************************************
	*
	* Display company name, program name, and version.
	*
	**************************************************************/
	
	printf("                 Lattice Semiconductor Corp.\n");
	printf("                   CPU Embedded v%s 2008\n\n", CPU_VERSION);
	
	/**************************************************************
	*
	* Assign arguments to global variables.
	*
	**************************************************************/
	
	g_pucDataArray = a_pucDataArray;
	g_ulDataSize = a_ulDataSize;

	/**************************************************************
	*
	* Calculate the file CRC.
	*
	**************************************************************/
	
	g_usCalculatedCRC = calculate_crc(g_ulDataSize, g_ulCurrentByteIndex);
	
	/**************************************************************
	*
	* Call cpu_engine to process the embedded bitstream data.
	*
	**************************************************************/
	
	cRetCode = cpu_engine();
	
	if (cRetCode < 0)
	{
		printf("\nFailed due to %s\n", error_handler(cRetCode));
        printf("+=======+\n");
        printf("| FAIL! |\n");
        printf("+=======+\n\n");
	} 
	else 
	{
		printf("\n+=======+\n");
        printf("| PASS! |\n");
        printf("+=======+\n\n");
	}
	
	return cRetCode;
}

unsigned char get_byte()
{
	/**************************************************************
	*
	* Return a single byte from the embedded bitstream array.
	*
	**************************************************************/
	
	if (g_ulCurrentByteIndex >= g_ulDataSize)
	{
		/**************************************************************
		*
		* If array has reached the end, return 0x00.
		*
		**************************************************************/
		
		return 0x00;
	}
	
	return (g_pucDataArray[g_ulCurrentByteIndex++]);
}





int storeBitstreamPointer()
{
/**************************************************************
*
* This is a function made for the user to store the current 
* bitstream pointer location.  It is to be used in conjunction
* with restoreBitstreamPointer().  
*
**************************************************************/
	
	storedCurrentByte = g_ulCurrentByteIndex;
	
	return 0;
}

int restoreBitstreamPointer()
{
/**************************************************************
*
* Resets the current file pointer to the previously stored 
* bitstream location from the function call storeBitstreamPointer()
*
**************************************************************/
	
	g_ulCurrentByteIndex = storedCurrentByte;
	
	return 0;
}

char * error_handler(signed char a_cRetCode)
{
	/**************************************************************
	*
	* Return error message based on the value of the return code.
	*
	**************************************************************/
	
	char * pszErrorMessage[] = 
	{
		"pass",
			"insufficient arguments",
			"file not found",
			"file error",
			"verification failure",
			"version error",
			"crc failure" 
	};
	
	return pszErrorMessage[-a_cRetCode];
}

unsigned long getCurrentbyteIndex()
{
	return g_ulCurrentByteIndex;
}

int setCurrentbyteIndex(unsigned long byteIndex)
{
	g_ulCurrentByteIndex = byteIndex;
	
	return 0;
}

unsigned short getUsCalculatedCRC()
{
	return g_usCalculatedCRC;
}