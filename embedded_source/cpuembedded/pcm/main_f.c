/**************************************************************
*
* Lattice Semiconductor Corp.
* 
* CPU Embedded allows programming of Lattice's suite of FPGA
* devices on embedded systems through the sysCONFIG port.  The software
* is distributed in source code form and is open to re - distribution
* and modification where applicable.
*
* This is the file - based main for the CPU Embedded.  This 
* version requires the embedded bitstream to be in binary format
*(with extension *.cpu).  The bitstream file shall be passed
* in as a parameter when calling the executable.
*
***************************************************************/
/* History*****************************************************
* CPU Embedded V1.2 4 - 27 - 06
* UPDATE
* 1   Changed to make the codes file - based processing to support 16 bits compiler
*	   Rename file cpu_definitions.h to cpu_code.h
*	   Rename file cpu_engine.c to cpu_core.c
*	   Rename file cpu_hardware.c to cpu_hard.c
*	   Rename file cpu_main_file.c to main_f.c
*	   Rename file cpu_main_compile.c to main_e.c
*	   Rename file cpu_main_function.c to main_tag.c
* 2   Changed the verify function to make it platform independent.
* 3   Added CPU file emulation
*	   Add file cpu_sim.c
*
* CPU Embedded V2.0.0 7 - 07 - 06
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
* Local variable declarations.
*
**************************************************************/

unsigned long g_ulDataSize = 0;
unsigned long g_ulCurrentByteIndex = 0;
FILE * pEmbeddedBitstreamFile;   
unsigned short g_usCalculatedCRC = 0x0000;
unsigned long storedCurrentByte = 0;


signed char main(int argc, char * argv[])
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
	* Check for valid number of arguments.
	*
	**************************************************************/
	
	if (argc != 2)
	{
		printf("\nUsage: cpu < embedded bitstream >\n");
		exit(CPU_INSUFFICIENT_ARGUMENTS);
	}
	
	/**************************************************************
	*
	* Open file pointer to the embedded bitstream file.
	*
	**************************************************************/
	
	pEmbeddedBitstreamFile = fopen(argv[1], "rb");
	if (!pEmbeddedBitstreamFile)
	{
		printf("File %s does not exists.\n", argv[1]);
		return CPU_FILE_NOT_FOUND;
	}
	
	/**************************************************************
	*
	* First pass through the embedded bitstream is to count the number
	* of bytes in the bitstream.
	*
	**************************************************************/
	
	while (getc(pEmbeddedBitstreamFile) != EOF) 
	{
		g_ulDataSize++;
	}
	
	/**************************************************************
	*
	* Rewind the file back to the beginning.
	*
	**************************************************************/
	
	rewind(pEmbeddedBitstreamFile); 
	
	
	/**************************************************************
	*
	* Calculate the file CRC.
	*
	**************************************************************/
	
	g_usCalculatedCRC = calculate_crc(g_ulDataSize, g_ulCurrentByteIndex);
	
	/**************************************************************
	*
	* Rewind the file back to the beginning.
	*
	**************************************************************/
	g_ulCurrentByteIndex = 0;
	rewind(pEmbeddedBitstreamFile);
	
	
	/**************************************************************
	*
	* Call cpu_engine to process the embedded bitstream file.
	*
	**************************************************************/
	
	cRetCode = cpu_engine();
	
	/**************************************************************
	*
	* Close file pointer to the embedded bitstream file.
	*
	**************************************************************/
	
	fclose(pEmbeddedBitstreamFile);
	pEmbeddedBitstreamFile = 0;
	
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
	unsigned char ByteData;
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
	
	ByteData = fgetc(pEmbeddedBitstreamFile);
	if (feof(pEmbeddedBitstreamFile))
	{            
		return 0x00;     
    }
	g_ulCurrentByteIndex++;
	return (ByteData);
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
	
	int result = fseek(pEmbeddedBitstreamFile, storedCurrentByte, SEEK_SET);
	g_ulCurrentByteIndex = storedCurrentByte;
	
	return (result);
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



