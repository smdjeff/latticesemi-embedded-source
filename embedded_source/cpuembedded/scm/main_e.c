/**************************************************************
*
* Lattice Semiconductor Corp.
* 
* CPU Embedded allows programming of Lattice's suite of FPGA
* devices on embedded systems through the sysCONFIG port.  The software
* is distributed in source code form and is open to re - distribution
* and modification where applicable.
*
* This is the compile - based main for the CPU Embedded.  This 
* version requires the embedded bitstream to be in C - code format.
* The bitstream file shall be compiled along with the cpu
* engine so that the generated executable contains the data.
*
**************************************************************/
/**************************************************************
* History
*
* CPU Embedded V1.2 9 - 04 - 07
* 1 Added CPU_COMMAND_VERIFY_REGISTER(0x0E) opcode to support SRAM Verify
*
*CPU Embedded V1.2 3 - 28 - 06
*========================
*UPDATE
*1	Supported FLASH and XFLASH CPU Programming.  
*2	Supported "Erase,Program,Verify", "Erase,Program,Verify and Secure", "Erase Only", "Verify Only" and "Verify ID" Operations.
*3   Added CPU_COMMAND_READ_REGISTER(0x0A) opcode to support Read and Verify
*4   Added CPU_COMMAND_HIGH_SPEED_CLK(0x0B) opcode to support the required fast CCLK.
*
*
*CPU Embedded V1.1 8 - 9 - 04
*========================
*UPDATE
*1	CFG pins support.  The embedded bitstreams may be generated with commands to set the CFG pins.  This allows
*    the user to specify the programming mode(Master PCM, Slave PCM, etc).  By default, the CFG pins shall not
*    be specified.  The CPU Embedded source code has been updated accordingly to process commands that drive the
*    CFG pins.
*2	BUSY pin support.  The embedded bitstreams may be generated with commands to check the BUSY pin.  This is
*    required to program FLASH devices.  The CPU Embedded source code has been updated accordingly to process
*    commands that check the BUSY pin.
*
* CPU Embedded V2.0.0 7 - 07 - 06
*========================
* UPDATE
* 1   Added support for compressed embedded bitstream file
* 2   Moved function declarations into central header file - cpu_head.h
* 3   Removed extern variable calls and replaced them with get() and set() functions
***************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "cpu_code.h"
#include "cpu_head.h"

/**************************************************************
*
* External variable declarations.
*
**************************************************************/

extern const unsigned long g_ulDataSize;     /*** Declared in the embedded bitstream C - code file. ***/
extern const unsigned char g_pucDataArray[]; /*** Declared in the embedded bitstream C - code file. ***/
unsigned long g_ulCurrentByteIndex = 0;
unsigned long storedCurrentByte = 0;

unsigned short g_usCalculatedCRC = 0x0000;

signed char main()
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
	
	if (
		cRetCode < 0)
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
	
	if (
		g_ulCurrentByteIndex >= g_ulDataSize)
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