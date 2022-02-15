/**************************************************************
*
* Lattice Semiconductor Corp. Copyright 2006 
* 
* CPU Embedded allows programming of Lattice's suite of FPGA
* devices on embedded systems through the sysCONFIG port.  The software
* is distributed in source code form and is open to re - distribution
* and modification where applicable.
* 
* 4/25/06 ht.
* This file is modified to simulate the processing of the CPU file
* by priniting out to the monitor the CPU vector file seen by the device.
* 1, 0 means drive to the device pin a High or Low signal respectively.
* H, L means read from the device pin a High or Low signal respectively.
* X means read back an unknown value.
* N means the pin is not driven by the cpu file.
* z means the pin is not driven to prepare for bus turn - around for reading. 
* ? means the pin is being tested for looping.
* Wait xxxxus or xxxxms means wait for that time for xxxx micro - seconds or millie - seconds.
* Loop XXXX means perform the following loop xxxx amount of time.
* 
*
**************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "cpu_code.h"
#include "cpu_head.h"


/**************************************************************
*
* Pin declarations.  The user may assign unused pins the value
* 0x0000.
*
**************************************************************/

const unsigned short g_din_pin					= 0x01;
const unsigned short g_cclk_pin					= 0x02;
const unsigned short g_init_pin					= 0x04;
const unsigned short g_prog_pin					= 0x08;
const unsigned short g_done_pin					= 0x10;
const unsigned short g_dout_pin					= 0x20;

/**************************************************************
*
* g_usRegister is a 8 - bit register that shall store the pin values.
* The bit orientation is defined above in the pin declarations.
* The control pins are grouped into the MSB and the data pins
* are grouped into the LSB.  The user may re - define the bit orientation
* to match the hardware.
*
* Note: user must re - implement to target specific hardware.
*
**************************************************************/

unsigned short g_usRegister = 0xFF;

/**************************************************************
*
* g_ucCFGRegister is an 8 - bit register that shall store the CFG
* pin values.  This register supports up to eight CFG pins, with
* CFG7 as MSB and CFG0 as LSB.  This value may be set by the
* embedded bitstream if it was generated with this option.  By 
* default, it is generated without this value.
*
**************************************************************/

unsigned char g_ucCFGRegister = 0xFF;


/**************************************************************
*
* Add loop_count to emulate a device responding to the interrogation.
* Add sim_start to show the first launch of this file.
* Add fpga_device to represent the pins of the device.
*
**************************************************************/

static int loop_count = 0; 
static char sim_bit_start = 1;
static char sim_verify_start = 1;
char fpga_device[20];  


int out_port(unsigned short a_usData)
{
/**************************************************************
*
* Function to write data out.  User may have the differentiate
* if a_usData is targeting the control pins, configuration pins,
* or data pins.
*
* Note: user must re - implement to target specific hardware.
*
**************************************************************/
	
	/*4/25/06 ht. Check if it is the first launch, then print the header*/
	char loop_var;
	
    if (sim_bit_start)
	{
		printf("1,0 means drive to the device pin a High or Low signal respectively.\n");
		printf("H,L means read from the device pin a High or Low signal respectively.\n");
		printf("X means read back an unknown value.\n");
		printf("N means the pin is not driven by the cpu file.\n");
		printf("z means the pin is not driven to prepare for bus turn-around for reading.\n"); 
		printf("? means the pin is being tested for looping.\n");
		printf("Wait xxxxus or xxxxms means wait for that time for xxxx micro-seconds or millie-seconds.\n");
		printf("Loop XXXX means perform the following loop xxxx amount of time.\n\n");
		
		printf("C D P I D \n");
		printf("C I R N O \n");
		printf("L N O I N \n");
		printf("K | G T E \n");
		printf("| | N N | \n");
		printf("| | | | | \n");
		sim_bit_start = 0;
	}
	
	/* _outp(g_data_address, a_usData); */
	for (loop_var = 0; loop_var < 11; loop_var++)
		fpga_device[loop_var] = ' ';
	fpga_device[loop_var++] = 0; /*string terminator*/
	if (a_usData & g_cclk_pin) 
		fpga_device[0] = '1';
	else 
		fpga_device[0] = '0';
	
	loop_var = 0;
	if (a_usData & g_din_pin)
		fpga_device[loop_var += 2]= '1';
	else 
		fpga_device[loop_var += 2] = '0';   
	
	if (a_usData & g_prog_pin) 
		fpga_device[loop_var += 2]= '1';
	else 
		fpga_device[loop_var += 2] = '0';
	
	fpga_device[loop_var += 2] = 'N';
	fpga_device[loop_var += 2] = 'N';     
	printf("%s\n", &fpga_device[0]);
	
	return 0;
}
unsigned short in_port()
{
	unsigned short usReturnData = g_usRegister;
	int loop_var;
	
	/**************************************************************
	*
	* Function to read data from location a_usAddress.
	*
	* Note: user must re - implement to target specific hardware.
	*
	**************************************************************/
	
	if (sim_verify_start)
	{
		printf("C D P I D \n");
		printf("C I R N O \n");
		printf("L N O I N \n");
		printf("K | G T E \n");
		printf("| | N N | \n");
		printf("| | | | | \n");
		sim_verify_start = 0;
	}
	
	
	for (loop_var = 0; loop_var < 11; loop_var++)
		fpga_device[loop_var] = ' ';
	fpga_device[loop_var++] = 0; /*string terminator*/
	if (usReturnData & g_cclk_pin) 
		fpga_device[0] = '1';
	else 
		fpga_device[0] = '0';
	loop_var = 0;
	if (usReturnData & g_din_pin)
		fpga_device[loop_var += 2]= 'H';
	else fpga_device[loop_var += 2] = 'L';   
	
	if (usReturnData & g_prog_pin) 
		fpga_device[loop_var += 2]= '1';
	else 
		fpga_device[loop_var += 2] = '0'; 
	fpga_device[loop_var += 2] = 'N';
	fpga_device[loop_var += 2] = 'N';     
	printf("%s\n", &fpga_device[0]);
	return usReturnData;
}

int set_cfg_register(unsigned short a_usCFGPinCount, unsigned char a_usCFGRegister)
{
/**************************************************************
*
* Assign the CFG register and immediately send it out to the 
* port to recognize the configuration mode.  User may need to
* use the argument a_usCFGPinCount to indicate how many CFG pins
* are used in the device.
*
* Note: user must re - implement to target specific hardware.
* 4/25/06 ht Users are strongly urged not to use the vector to drive
*            the CFGx pins. They should statically pull - down or pull - up on
*            the boards. Dynamic swicthing of those pins can cause boot up
*            challenge. So simulation is not supported here, instead a
*            warning message is issued when the vector setting CFGx pin is
*            detected.
*
	**************************************************************/
	
	g_ucCFGRegister = a_usCFGRegister;
	
    /*4/25/06 ht Comment out and replace it by a warning message*/
	
	/* out_port(g_ucCFGRegister); */
    printf("WARNING: The vector setting the CFGx pin(s) is detected. Please handle it carefully in your code\n");
	
	return 0;
}
int set_delay(unsigned long a_ulDelay)
{
/**************************************************************
*
* The user must implement a delay to observe a_ulDelay, where
* a_ulDelay is the number of micro - seconds that must pass before
* data is read from in_port.  Since platforms and processor speeds
* vary greatly, this task is left to the user.
*
* Note: user must re - implement to target specific hardware.
*
	**************************************************************/
	
	return 0;
}

signed char check_pin(unsigned char a_ucPin, unsigned char a_ucPinValue, unsigned long a_ulDelay, unsigned long a_ulRepetition)
{
	signed char scRetCode = CPU_SUCCESS;
	unsigned short usReadValue = 0x0000;
	unsigned short usPinToVerify = 0x0000;
    char loop_var;     /*4/25/06 ht Added for indexing*/
	/*4/25/06 ht. Put the N on the done pin column.*/
	fpga_device[8] = 'N';
	/*4/25/06 ht. Put the N on the init pin column.*/
	fpga_device[6] = 'N';	
	/**************************************************************
	*
	* Check to see if the pin is being driven to a valid value.
	*
	**************************************************************/
	if ((a_ucPinValue != CPU_LOW) &&(a_ucPinValue != CPU_HIGH))
		return (CPU_INVALID_FILE);
	
	
		/**************************************************************
		*
		* Check to see which pin is going to be sensed.
		*
	**************************************************************/
	
	switch (a_ucPin) 
	{
		case CPU_DONE_PIN:
			
		/**************************************************************
		*
		* Done pin.
		*
			**************************************************************/
			
			usPinToVerify = g_done_pin;
			/*4/25/06 ht. Put the ? on the done pin column to indicate it is being tested.*/
			loop_var = 8;
			fpga_device[loop_var] = '?';
			break;
		case CPU_INIT_PIN:
			
		/**************************************************************
		*
		* Init pin.
		*
			**************************************************************/
			
			usPinToVerify = g_init_pin;
			/*4/25/06 ht. Put the ? on the init pin column to indicate it is being tested.*/
			loop_var = 6;
			fpga_device[loop_var] = '?';
			break;
		default:
			
		/**************************************************************
		*
		* Invalid sensed value.
		*
			**************************************************************/
			
			return CPU_INVALID_FILE;
	}
	
    /*4/25/06 ht. Looping begin here.*/
    printf("Loop %ld\n" , a_ulRepetition);
    printf("%s\n", &fpga_device[0]);
	loop_count = 0;
	while (a_ulRepetition-- > 0) 
	{
		if (a_ulDelay > 0)
		{
			set_delay(a_ulDelay);
            printf("Wait %ld us\n", a_ulDelay);  /*4/25/06 ht. Indicate the wait time*/
		}
		/**************************************************************
		*
		* Read data from the input port.
        * 4/25/06 ht. For simulation purpose, the code is made to loop twice then force
        *             a pass so that the rest of the vectors can be simulated.
		*
		**************************************************************/
		
		/* usReadValue = in_port(false); */
        if (loop_count < 2) 
		{
			if (a_ucPinValue)
				usReadValue = !usPinToVerify; /*4/25/06 ht. Force a fail on the first two loop*/
			else
				usReadValue = usPinToVerify;
		}
        else 
		{
			if (a_ucPinValue)
				usReadValue = usPinToVerify;
			else
				usReadValue = !usPinToVerify;
		}
		loop_count++;     
        if (usReadValue) 
			fpga_device[loop_var] = 'H'; /*4/25/06 ht. Show the value read on the pin being tested*/
        else 
			fpga_device[loop_var] = 'L'; 
        printf("%s\n", &fpga_device[0]);        /*4/25/06 ht. Print it out to emulate*/
		
		if (a_ucPinValue)
		{
			if (usReadValue & usPinToVerify)
			{
				scRetCode = CPU_SUCCESS;
				break;
			}
			else 
			{
				scRetCode = CPU_VERIFICATION_FAILURE;
			}
		}
		else 
		{
			if (usReadValue & usPinToVerify)
			{
				scRetCode = CPU_VERIFICATION_FAILURE;
			}
			else 
			{
				scRetCode = CPU_SUCCESS;
				break;
			}
		}
	}
	
	return scRetCode;
}


signed char set_pin(unsigned char a_ucPin, unsigned char a_ucPinValue)
{
/**************************************************************
*
* Check to see if the pin is being driven to a valid value.
*
	**************************************************************/
	if ((a_ucPinValue != CPU_LOW) &&(a_ucPinValue != CPU_HIGH))
		return (CPU_INVALID_FILE);
	
	
	switch (a_ucPin) 
	{
		case CPU_PROG_PIN:
			
		/**************************************************************
		*
		* Program pin.
		*
		**************************************************************/
			
			if (a_ucPinValue)
			{
				g_usRegister |= g_prog_pin;
			}
			else 
			{
				g_usRegister &= ~g_prog_pin;
			}
			break;
		case CPU_CCLK_PIN:
			
		/**************************************************************
		*
		* CCLK pin.
		*
		**************************************************************/
			
			if (a_ucPinValue)
			{
				g_usRegister |= g_cclk_pin;
			}
			else 
			{
				g_usRegister &= ~g_cclk_pin;
			}
			break;
		case CPU_DIN_PIN:
			
		/**************************************************************
		*
		* Din pin.
		*
		**************************************************************/
			
			if (a_ucPinValue)
			{
				g_usRegister |= g_din_pin;
			}
			else 
			{
				g_usRegister &= ~g_din_pin;
			}
			break;
		default:
			
		/**************************************************************
		*
		* Invalid pin.
		*
		**************************************************************/
			
			return CPU_INVALID_FILE;
	}
	
	return CPU_SUCCESS;
}
unsigned short getUsRegister()
{
	return g_usRegister;
}


