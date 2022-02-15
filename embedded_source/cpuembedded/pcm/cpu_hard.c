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

#include <stdio.h>
#include <stdlib.h>

#include "cpu_code.h"
#include "cpu_head.h"

/**************************************************************
*
* Address declarations.
*
* Note: user must re - implement to target specific hardware.
*
**************************************************************/

const unsigned short g_data_address		    = 0;
const unsigned short g_status_address		= 0;

/**************************************************************
*
* Pin declarations.  The user may assign unused pins the value
* 0x0000.
*
**************************************************************/

const unsigned short g_d0_pin					= 0x0001;
const unsigned short g_d1_pin					= 0x0002;
const unsigned short g_d2_pin					= 0x0004;
const unsigned short g_d3_pin					= 0x0008;
const unsigned short g_d4_pin					= 0x0010;
const unsigned short g_d5_pin					= 0x0020;
const unsigned short g_d6_pin					= 0x0040;
const unsigned short g_d7_pin					= 0x0080;
const unsigned short g_prog_pin					= 0x0100;
const unsigned short g_init_pin					= 0x0200;
const unsigned short g_done_pin					= 0x0400;
const unsigned short g_cs_pin					= 0x0800;
const unsigned short g_cs1_pin					= 0x1000;
const unsigned short g_write_en_pin				= 0x2000;
const unsigned short g_cclk_pin					= 0x4000;
const unsigned short g_busy_pin					= 0x8000;


/**************************************************************
*
* g_usRegister is a 16 - bit register that shall store the pin values.
* The bit orientation is defined above in the pin declarations.
* The control pins are grouped into the MSB and the data pins
* are grouped into the LSB.  The user may re - define the bit orientation
* to match the hardware.
*
* Note: user must re - implement to target specific hardware.
*
**************************************************************/

unsigned short g_usRegister = 0xFFFF;

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
	
	/* _outp(g_data_address, a_usData); */
	
	return 0;
}
unsigned short in_port()
{
/**************************************************************
*
* Function to read data from location a_usAddress.
*
* Note: user must re - implement to target specific hardware.
*
**************************************************************/
	
	unsigned short usReturnData = 0x0000;
	/* usReturnData = _inp(g_status_address); */
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
*
**************************************************************/
	
	g_ucCFGRegister = a_usCFGRegister;
	
	out_port(g_ucCFGRegister);
	
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
* This subroutine is called upon to provide a delay from 1 millisecond
* to a few hundreds milliseconds each time. That is the
* reason behind using unsigned long integer in this subroutine.
* It is OK to provide longer delay than required. It is not
* acceptable if the delay is shorter than required. 
*
* Note: user must re - implement to target specific hardware.
*
* Example: Use the for loop to create the micro-second delay.
*  
*          Let the CPU clock (system clock) be F Mhz. 
*                                   
*          Let the for loop represented by the 2 lines of machine code:
*                    LOOP:  DEC RA;
*                           JNZ LOOP;
*          Let the for loop number for one micro-second be L.
*          Lets assume 4 system clocks for each line of machine code.
*          Then 1 us = 1/F (micro-seconds per clock) 
*                       x (2 lines) x (4 clocks per line) x L
*                     = 8L/F          
*          Or L = F/8;
*        
*          Lets assume the CPU clock is set to 48MHZ. The C code then is:
*
*          unsigned long F = 48;  
*          unsigned long L = F/8;
*          unsigned long index;
*          if (L < 1) L = 1;
*          for (index=0; index < a_ulDelay * L; index++);
*          return 0;
*           
*
**************************************************************/
	
	return 0;
}


signed char check_pin(unsigned char a_ucPin, unsigned char a_ucPinValue, unsigned long a_ulDelay, unsigned long a_ulRepetition)
{
	signed char scRetCode = CPU_SUCCESS;
	unsigned short usReadValue = 0x0000;
	unsigned short usPinToVerify = 0x0000;
	
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
			break;
		case CPU_INIT_PIN:
			
		/**************************************************************
		*
		* Init pin.
		*
		**************************************************************/
			
			usPinToVerify = g_init_pin;
			break;
		case CPU_BUSY_PIN:
			
		/**************************************************************
		*
		* Busy pin.
		*
		**************************************************************/
			
			usPinToVerify = g_busy_pin;
			break;
		default:
			
		/**************************************************************
		*
		* Invalid sensed value.
		*
		**************************************************************/
			
			return CPU_INVALID_FILE;
	}
	
	while (a_ulRepetition-- > 0) 
	{
		if (a_ulDelay > 0)
		{
			set_delay(a_ulDelay);
		}
		/**************************************************************
		*
		* Read data from the input port.
		*
		**************************************************************/
		
		usReadValue = in_port();
		
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
	
		/**************************************************************
		*
		* Check to see which pin is going to be driven.
		*
	    **************************************************************/
	
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
		case CPU_CS_PIN:
			
		/**************************************************************
		*
		* CS pin.
		*
		**************************************************************/
			
			if (a_ucPinValue)
			{
				g_usRegister |= g_cs_pin;
			}
			else 
			{
				g_usRegister &= ~g_cs_pin;
			}
			break;
		case CPU_CS1_PIN:
			
		/**************************************************************
		*
		* CS1 pin.
		*
		**************************************************************/
			
			if (a_ucPinValue)
			{
				g_usRegister |= g_cs1_pin;
			}
			else 
			{
				g_usRegister &= ~g_cs1_pin;
			}
			break;
		case CPU_WRITE_EN_PIN:
			
		/**************************************************************
		*
		* Write enable pin.
		*
		**************************************************************/
			
			if (a_ucPinValue)
			{
				g_usRegister |= g_write_en_pin;
			}
			else 
			{
				g_usRegister &= ~g_write_en_pin;
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

int set_byte_data(unsigned char a_ucData)
{
/**************************************************************
*
* Set value for D7.
*
**************************************************************/
	
	if (a_ucData & 0x01)
	{
		g_usRegister |= g_d7_pin;
	}
	else 
	{
		g_usRegister &= ~g_d7_pin;
	}
	
	/**************************************************************
	*
	* Set value for D6.
	*
	**************************************************************/
	
	if (a_ucData & 0x02)
	{
		g_usRegister |= g_d6_pin;
	}
	else 
	{
		g_usRegister &= ~g_d6_pin;
	}
	
	/**************************************************************
	*
	* Set value for D5.
	*
	**************************************************************/
	
	if (a_ucData & 0x04)
	{
		g_usRegister |= g_d5_pin;
	}
	else 
	{
		g_usRegister &= ~g_d5_pin;
	}
	
	/**************************************************************
	*
	* Set value for D4.
	*
	**************************************************************/
	
	if (a_ucData & 0x08)
	{
		g_usRegister |= g_d4_pin;
	}
	else 
	{
		g_usRegister &= ~g_d4_pin;
	}
	
	/**************************************************************
	*
	* Set value for D3.
	*
	**************************************************************/
	
	if (a_ucData & 0x10)
	{
		g_usRegister |= g_d3_pin;
	}
	else 
	{
		g_usRegister &= ~g_d3_pin;
	}
	
	/**************************************************************
	*
	* Set value for D2.
	*
	**************************************************************/
	
	if (a_ucData & 0x20)
	{
		g_usRegister |= g_d2_pin;
	}
	else 
	{
		g_usRegister &= ~g_d2_pin;
	}
	
	/**************************************************************
	*
	* Set value for D1.
	*
	**************************************************************/
	
	if (a_ucData & 0x40)
	{
		g_usRegister |= g_d1_pin;
	}
	else 
	{
		g_usRegister &= ~g_d1_pin;
	}
	
	/**************************************************************
	*
	* Set value for D0.
	*
	**************************************************************/
	
	if (a_ucData & 0x80)
	{
		g_usRegister |= g_d0_pin;
	}
	else 
	{
		g_usRegister &= ~g_d0_pin;
	}
	
	return 0;
}

unsigned short getUsRegister()
{
	return g_usRegister;
}
