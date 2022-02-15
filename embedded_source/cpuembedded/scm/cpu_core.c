/**************************************************************
*
* Lattice Semiconductor Corp.
* 
* CPU Embedded allows programming of Lattice's suite of FPGA
* devices on embedded systems through the sysCONFIG port.  The software
* is distributed in source code form and is open to re - distribution
* and modification where applicable.
*
**************************************************************/

/*************************************************************************
*
* Warning: this file should not be modified otherwise the cpu_engine
* may decode the embedded bitstream incorrectly.
*
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu_code.h"
#include "cpu_head.h"


static int compressed = 0;

 
signed char cpu_engine()
{
	signed char scRetCode = CPU_SUCCESS;
	unsigned char ucCommand = 0x00;
	
	while ((ucCommand = get_byte()) != CPU_COMMAND_END_BYTE) 
	{
	/**************************************************************
	*
	* Switch is based on the commands in the embedded bitstream.
	*
	**************************************************************/
		
		switch (ucCommand)
		{
			case CPU_COMMAND_BIT_DATA:
				
			/**************************************************************
			*
			* Send one frame of bitstream data byte by byte.
			* Outputs in serial (bit by bit) format
			*
			**************************************************************/
				
				command_bit_data();
				break;
			case CPU_COMMAND_COMMENT:
				
			/**************************************************************
			*
			* Display the comment in the embedded bitstream file.
			*
			**************************************************************/
				
				command_comment();
				break;
			case CPU_COMMAND_SET_PIN:
				
			/**************************************************************
			*
			* Set pin to a certain value to be driven.
			*
			**************************************************************/
				
				scRetCode = command_set_pin();
				break;
			case CPU_COMMAND_SEND_REGISTER:
				
			/**************************************************************
			*
			* Send the register to the output port.
			*
			**************************************************************/
				
				out_port(getUsRegister());
				break;
			case CPU_COMMAND_CHECK_PIN:
				
			/**************************************************************
			*
			* Check pin against specified value.
			*
			**************************************************************/
				
				scRetCode = command_check_pin();
				break;
			case CPU_COMMAND_VERSION:
				
			/**************************************************************
			*
			* Check to make sure the embedded bitstream version is compatible
			* with the cpu_engine.
			*
			**************************************************************/
				
				scRetCode = command_version();
				break;
			case CPU_COMMAND_COMPRESS:
				
			/**************************************************************
			*
			* Indicates the embedded bitstream file is compressed.
			*
			**************************************************************/
				
				command_compress();
				break;
			case CPU_COMMAND_CRC:
				
			/**************************************************************
			*
			* Verify the crc of the embedded bitstream.
			*
			**************************************************************/
				
				scRetCode = command_crc();
				break;
			case CPU_COMMAND_SET_CFG_REGISTER:
				
			/**************************************************************
			*
			* Set the CFG register.
			*
			**************************************************************/
				
				command_set_cfg_register();
				break;
			default:
				
			/**************************************************************
			*
			* Invalid command.  Return immediately.
			*
			**************************************************************/
				
				scRetCode = CPU_INVALID_FILE;
				break;
		}
		
		if (scRetCode != CPU_SUCCESS)
		{
			return scRetCode;
		}
	}
	
	return scRetCode;
}

unsigned short get_number()
{
	unsigned short usiNumber = 0;
	unsigned char usCurrentByte, ucIndex = 0;
	
	/**************************************************************
	*
	* De - code the number and return it in the variable usiNumber.
	*
	**************************************************************/
	
	while ((usCurrentByte = get_byte()) & 0x80) 
	{
		usiNumber |=((unsigned short)(usCurrentByte & 0x7F)) << ucIndex;
		ucIndex += 7;
	}
	usiNumber |=((unsigned short)(usCurrentByte & 0x7F)) << ucIndex;
	
	return usiNumber;
}

int command_comment()
{
	unsigned short usCommentSize = get_number();
	
	/**************************************************************
	*
	* Iterate through the size of the comment and print it out to
	* the terminal.
	*
	**************************************************************/
	
	printf("Processing: ");
	for (; usCommentSize > 0; usCommentSize--)
	{
		printf("%c", get_byte());
	}
	printf("...\n");
	
	return 0;
}


/**************************************************************
*
*  outputs bit by  bit to Din pin, instead of writing a byte 
*  to D0-D7 pins.
*
***************************************************************/
int command_bit_data()
{
	unsigned short size;
	unsigned short loop_var0, loop_var1, loop_var2, getData = 0;
	unsigned char cDataByte, compress = 0;
	unsigned short FFcount = 0;
	unsigned char compr_char = 0xFF; 
	unsigned short index;
	signed char compression = 0;
	unsigned short usDataSize = 0x0000;
	unsigned char *ByteData = 0;
	
	/*************************************************************
	*
	* Get the size of the data.
	*
	**************************************************************/
	
	usDataSize = get_number();
	
	/**************************************************************
	*
	* Decompression scheme 
	* The decompression scheme employed in this module
    * is on row by row basis. The format of the data stream:
	*[compression code][compressed data stream]
	* 0x00    --No compression
	* 0x01    --Compress by 0x00.
	*           Example:
	*           Original stream:   0x000000000000000000000001
	*           Compressed stream: 0x01000901
	*           Detail:            0x01 is the code, 0x00 is the key, 
	*                              0x09 is the count of 0x00 bytes,
	*                              0x01 is the uncompressed byte.
	* 0x02    --Compress by 0xFF.
	*           Example:
	*           Original stream:   0xFF FF FF FF FF FF FF FF FF FF FF 01
	*           Compressed stream: 0x02FF0901
	*           Detail:            0x02 is the code, 0xFF is the key, 
	*                              0x09 is the count of 0xFF bytes,
	*                              0x01 is the uncompressed byte.
	* 0x03
	* : :
	* 0xFE   -- Compress by nibble blocks.
	*           Example:
	*           Original stream:   0x84210842108421084210
	*           Compressed stream: 0x0584210
	*           Detail:            0x05 is the code, means 5 nibbles block.
	*                              0x84210 is the 5 nibble blocks.
	*                              The whole row is 80 bits given by g_usiDataSize.
	*                              The number of times the block repeat itself
	*                              is found by g_usiDataSize/(4*0x05) which is 4.
	* 0xFF   -- Compress by the most frequently happen byte.
	*           Example:
	*           Original stream:   0x04020401030904040404
	*           Compressed stream: 0xFF04(0, 1, 0x02, 0, 1, 0x01, 1, 0x03, 1, 0x09, 0, 0, 0)
	*                          or: 0xFF044090181C240
	*           Detail:            0xFF is the code, 0x04 is the key.
	*                              a bit of 0 represent the key shall be put into
	*                              the current bit position and a bit of 1
	*                              represent copying the next of 8 bits of data
	*                              in.
	*
	***************************************************************/
	
	
	/**************************************************************
	*
	* Check if compression was enabled on the file
	*
	***************************************************************/
	if (compressed)
	{
		ByteData =(unsigned char*)malloc((usDataSize + 1) * sizeof(unsigned char));
		if(!ByteData)
			return CPU_OUT_MEMORY;

		size = usDataSize;
		
		compression = 1;
		compress = get_byte();
		switch (compress)
		{
				/**************************************************************
				*
				* No Compression
				*
				***************************************************************/
			case 0x00:  
				compression = 0; 
				break;
				/**************************************************************
				*
				* Compressed by 0x00 byte
				*
				***************************************************************/
			case 0x01:  
				compr_char = 0x00;
				break;
				/**************************************************************
				*
				* Compressed by 0xFF byte
				*
				***************************************************************/
			case 0x02: 
				compr_char = 0xFF;  
				break;
				/**************************************************************
				*
				* Compressed by the most frequently occuring byte
				*
				***************************************************************/
			case 0xFF: 
				compr_char = get_byte();
				loop_var0 = 8;
				for (index = 0; index < size; index++)
				{
					ByteData[index] = 0x00;
					if (loop_var0 > 7)
					{
						cDataByte = get_byte(); 
						loop_var0 = 0;
					}
					if ((cDataByte << loop_var0++) & 0x80) 
						loop_var2 = 8;
					else 
					{
						ByteData[index] = compr_char;
						loop_var2 = 0;
					}
					
					for (loop_var1 = 0; loop_var1 < loop_var2; loop_var1++)
					{
						if (loop_var0 > 7)
						{
							cDataByte = get_byte(); 
							loop_var0 = 0;
						}
						ByteData[index] |=((cDataByte << loop_var0++)&0x80) >> loop_var1;
					} 
				}     
				size = 0;
				break;
				/**************************************************************
				*
				* Compressed using Huffman encoding - nibble blocks
				*
				***************************************************************/
			default:   
				for (index = 0; index < size; index++) 
					ByteData[index] = 0x00;
				for (index = 0; index < compress; index++)
				{
					if (index%2 == 0) 
						cDataByte = get_byte();
					for (loop_var0 = 0; loop_var0 < size*2/compress; loop_var0++)
					{
						loop_var1 = index + loop_var0*compress;
						/*clear the nibble to zero first*/
						if (loop_var1%2)
						{
							if (index%2) 
								ByteData[loop_var1/2] |= cDataByte & 0x0F;
							else 
								ByteData[loop_var1/2] |= cDataByte >> 4;
						}
						else 
						{
							if (index%2) 
								ByteData[loop_var1/2] |= cDataByte << 4;
							else 
								ByteData[loop_var1/2] |= cDataByte & 0xF0;
						}
					}
				}
				size = 0;
				break;
		}
		
		
		
		FFcount = 0;
		
		/**************************************************************
		*
		* If the code is 0x00, 0xFF, or the data is uncompressed, 
		* this section decodes it and places it into the char array
		*
		***************************************************************/
		for (index = 0; index < size; index++) 
		{
			if (FFcount <= 0) 
			{
				cDataByte = get_byte();
				ByteData[index] = cDataByte; 
				if ((compression) &&(cDataByte == compr_char)) /*decompression is on*/
					FFcount =(unsigned short)get_number();     /*The number of 0xFF or 0x00 bytes*/
			}
			else 
			{
				FFcount--; /*Use up the 0xFF chain first*/
				ByteData[index] = compr_char;
			}
		}
		size = usDataSize;
		
		/**************************************************************
		*
		* Iterate through the char array and output to port
		*
		**************************************************************/
		for (usDataSize = 0; usDataSize < size; usDataSize++)
		{
		/**************************************************************
		*
		* Call function to map a byte of data to the data pins.
		*
		**************************************************************/
			
			//set_byte_data(ByteData[usDataSize]);
			unsigned char ucByte = ByteData[usDataSize];
			int byteItr;
			for(byteItr = 0; byteItr < 8; byteItr++){
				
				if(ucByte & 0x80)
					set_pin(CPU_DIN_PIN, CPU_HIGH);
				else
					set_pin(CPU_DIN_PIN, CPU_LOW);

				/**************************************************************
				*
				* Drive CCLK low and write the data out.
				*
				**************************************************************/
				
				set_pin(CPU_CCLK_PIN, CPU_LOW);
				out_port(getUsRegister());
				
				/**************************************************************
				*
				* Drive CCLK high and write the data out.
				*
				**************************************************************/
				
				set_pin(CPU_CCLK_PIN, CPU_HIGH);
				out_port(getUsRegister());

				ucByte <<= 1;

			}			
		}
	}
	else
	{
	/**************************************************************
	*
	* Iterate through the size of the data and write each byte 
	* to the port.
	*
	**************************************************************/
		for (; usDataSize > 0; usDataSize--)
		{
		/**************************************************************
		*
		* Call function to map a byte of data to the data pins.
		*
		**************************************************************/
			
			//set_byte_data(get_byte());	
			unsigned char ucByte = get_byte();
			int byteItr;
			for(byteItr = 0; byteItr < 8; byteItr++){
				
				if(ucByte & 0x80)
					set_pin(CPU_DIN_PIN, CPU_HIGH);
				else
					set_pin(CPU_DIN_PIN, CPU_LOW);
				/**************************************************************
				*
				* Drive CCLK low and write the data out.
				*
				**************************************************************/
				
				set_pin(CPU_CCLK_PIN, CPU_LOW);
				out_port(getUsRegister());
				
				/**************************************************************
				*
				* Drive CCLK high and write the data out.
				*
				**************************************************************/
				
				set_pin(CPU_CCLK_PIN, CPU_HIGH);
				out_port(getUsRegister());

				ucByte <<= 1;

			}
		}
	}
	
	/**************************************************************
	*
	* Drive CCLK low and write the data out.
	*
	**************************************************************/
	set_pin(CPU_CCLK_PIN, CPU_LOW);
	out_port(getUsRegister());
	if (ByteData)
		free(ByteData);
	ByteData = 0;
	
	return 0;
}

signed char command_version()
{
/**************************************************************
*
* The embedded bitstream and cpu_engine versions are compared
* by converting them into unsigned shorts.
*
* For example, if the embedded bitstream version is "1.0.3" and
* the cpu engine version is "1.1.0".  When converted to unsigned
* shorts, each number becomes 103 and 110, respectively.  Since
* the cpu engine is higher, it should be backward compatible thus
* it should support the embedded bitstream.
*
**************************************************************/
	
	unsigned char ucVersionIndex;
	unsigned char ucVersionByte;
	unsigned char ucCPUVersionIndex;
	
	unsigned short usVersionLength;
	unsigned short usBitstreamVersion;
	unsigned short usCPUEngineVersion;
	char * pszVersion = CPU_VERSION;
	char szBitstreamVersion[_MAX_VERSION_LENGTH_] = { 0 };
	char szCPUEngineVersion[_MAX_VERSION_LENGTH_] = { 0 };
	/**************************************************************
	*
	* Get the embedded bitstream version length.
	*
	**************************************************************/
	
	usVersionLength = get_number();
	
	/**************************************************************
	*
	* Hopefully the version length shall never exceed the maximum
	* version length.
	*
	**************************************************************/
	
	if (usVersionLength > _MAX_VERSION_LENGTH_)
	{
		return CPU_INVALID_VERSION;
	}
	
	/**************************************************************
	*
	* Build the embedded bitstream version.
	*
	**************************************************************/
	
	ucVersionIndex = 0;
	for (; usVersionLength > 0; usVersionLength--)
	{
		ucVersionByte = get_byte();
		if (ucVersionByte != '.')
		{
			szBitstreamVersion[ucVersionIndex++] = ucVersionByte;
		}
	}
	usBitstreamVersion = atoi(szBitstreamVersion);
	
	
	/**************************************************************
	*
	* Build the cpu engine version.
	*
	**************************************************************/
	
	ucVersionIndex = 0;
	for (ucCPUVersionIndex = 0; ucCPUVersionIndex < strlen(pszVersion); ucCPUVersionIndex++)
	{
		if (pszVersion[ucCPUVersionIndex] != '.')
		{
			szCPUEngineVersion[ucVersionIndex++] = pszVersion[ucCPUVersionIndex];
		}
	}
	usCPUEngineVersion = atoi(szCPUEngineVersion);
	
	
	/**************************************************************
	*
	* Compare the embedded bitstream version versus the cpu engine.
	*
	**************************************************************/
	
	if (usBitstreamVersion > usCPUEngineVersion)
	{
		return CPU_INVALID_VERSION;
	}
	
	return CPU_SUCCESS;
}

signed char command_set_pin()
{
	unsigned char ucPin = 0x00;
	unsigned char ucPinValue = 0x00;
	
	/**************************************************************
	*
	* Get the pin definition defined by the user.
	*
	**************************************************************/
	
	ucPin = get_byte();
	
	/**************************************************************
	*
	* Get the pin value opcode.
	*
	**************************************************************/
	
	ucPinValue = get_byte();
	
	/**************************************************************
	*
	* Call function to set the pin with its value.
	*
	**************************************************************/
	
	return (set_pin(ucPin, ucPinValue));
}

signed char command_check_pin()
{
	unsigned char ucPin;
	unsigned char ucPinValue;
	unsigned long ulDelay;
	unsigned long ulRepetition;
	
	/**************************************************************
	*
	* Get the pin opcode.
	*
	**************************************************************/
	
	ucPin = get_byte();
	
	/**************************************************************
	*
	* Get the pin value opcode.
	*
	**************************************************************/
	
	ucPinValue = get_byte();
	
	/**************************************************************
	*
	* Get the delay time.
	*
	**************************************************************/
	
	ulDelay = get_number();
	
	
	/**************************************************************
	*
	* Get the repetition time.
	*
	**************************************************************/
	
	ulRepetition = get_number();
	if (ulRepetition == 0x01)
	{
		ulDelay = ulDelay * 1000;
		ulRepetition = get_number();
	}
	
	/**************************************************************
	*
	* Call function to check the pin against its value.
	*
	**************************************************************/
	
	return (check_pin(ucPin, ucPinValue, ulDelay, ulRepetition));
}

signed char command_crc()
{
	unsigned short usExpectedCRC;
	
	/**************************************************************
	*
	* Get the expected crc.
	*
	**************************************************************/
	
	usExpectedCRC = get_byte();
	usExpectedCRC <<= 8;
	usExpectedCRC += get_byte();
	
	/**************************************************************
	*
	* Calculate the crc and return the value.
	*
	**************************************************************/
	
	if (usExpectedCRC != getUsCalculatedCRC())
	{
		return CPU_CRC_FAILURE;
	}
	
	return CPU_SUCCESS;
}


unsigned short calculate_crc(unsigned long g_ulDataSize, unsigned long g_ulCurrentByteIndex)
{
	unsigned char ucData;
	unsigned char ucTempData;
	unsigned char ucByteIndex;
	unsigned short usCRCTableEntry, usCalculatedCRC = 0x0000;
	unsigned long ulIndex, ulTemporaryByteIndex;
	unsigned short crc_table[16] = 
	{
		0x0000, 0xCC01, 0xD801,
			0x1400, 0xF001, 0x3C00,
			0x2800, 0xE401, 0xA001,
			0x6C00, 0x7800, 0xB401,
			0x5000, 0x9C01, 0x8801,
			0x4400
	};
	
	/**************************************************************
	*
	* Remember the current byte index.
	*
	**************************************************************/
	
	ulTemporaryByteIndex = g_ulCurrentByteIndex;
	get_byte();
	get_byte();
	get_byte();
	
	for (ulIndex = getCurrentbyteIndex(); ulIndex < g_ulDataSize; ulIndex++)
	{
	/**************************************************************
	*
	* Iterate through the data and calculate the crc for each byte.
	*
	**************************************************************/
		
		ucData = get_byte();
		
		/**************************************************************
		*
		* Flip byte.
		*
		**************************************************************/
		
		ucTempData = 0x00;
		for (ucByteIndex = 0; ucByteIndex < 8; ucByteIndex++)
		{
			ucTempData <<= 1;
			if (ucData & 0x1)
			{
				ucTempData |= 0x1;
			}
			ucData >>= 1;
		}
		ucData = ucTempData;
		
		/**************************************************************
		*
		* Perform calculation.
		*
		**************************************************************/
		
		usCRCTableEntry = crc_table[usCalculatedCRC & 0xF];
		usCalculatedCRC =(usCalculatedCRC >> 4) & 0x0FFF;
		usCalculatedCRC = usCalculatedCRC ^ usCRCTableEntry ^ crc_table[ucData & 0xF];
		usCRCTableEntry = crc_table[usCalculatedCRC & 0xF];
		usCalculatedCRC =(usCalculatedCRC >> 4) & 0x0FFF;
		usCalculatedCRC = usCalculatedCRC ^ usCRCTableEntry ^ crc_table[(ucData >> 4) & 0xF];
	}
	
	/**************************************************************
	*
	* Restore the current byte index.
	*
	**************************************************************/
	
	setCurrentbyteIndex(ulTemporaryByteIndex);
	// g_ulCurrentByteIndex = ulTemporaryByteIndex;
	
	return usCalculatedCRC;
}

int command_set_cfg_register()
{
/**************************************************************
*
* Get the number of CFG pins.
*
**************************************************************/
	
	unsigned short usCFGPinCount = get_number();
	
	/**************************************************************
	*
	* Process the CFG register byte.
	*
	**************************************************************/
	
	set_cfg_register(usCFGPinCount, get_byte());
	
	return 0;
}
int command_compress()
{
/**************************************************************
*
* Check if the file is compressed or not
*
**************************************************************/
	if (get_byte())
		compressed = 1;
	else
		compressed = 0;
	
	return 0;
}


