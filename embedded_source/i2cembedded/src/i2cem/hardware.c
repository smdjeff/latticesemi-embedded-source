/**************************************************************
*
* Lattice Semiconductor Corp. Copyright 2011
* 
*
***************************************************************/


/**************************************************************
* 
* Revision History of hardware.c
* 
* 
* Support v1.0
***************************************************************/
#include "opcode.h"

/*************************************************************
*                                                            *
* Opcode for discrete pins toggling							 *
*                                                            *
*************************************************************/
//#define signalSCL		  0x01    //I2C Bus
//#define signalSDA		  0x08    
//#define signalSCL_IN    0x0003    
//#define signalSDA_IN	  0x0004    
//#define signalTRST      0x80    
/********************************************************************************
* Declaration of global variables 
*
*********************************************************************************/
unsigned short g_usCpu_Frequency  = 1000;   /*Enter your CPU frequency here, unit in MHz.*/

/***************************************************************
*
* Functions declared in hardware.c module.
*
***************************************************************/
int ReadBytesAndSendNACK( int length, unsigned char *a_ByteRead, int NAck );
int SendBytesAndCheckACK(int length, unsigned char *a_bByteSend);
int ToggleTRST(int toggle);
void EnableHardware();
void DisableHardware();


/*************************************************************
*                                                            *
* ToggleTRST                                                 *
*                                                            *
* INPUT:                                                     *
*     CRESET signal value.                                   *
*                                                            *
* RETURN:                                                    *
*     int                                                   *
*                                                            *
* DESCRIPTION:                                               *
*     This function is used to set the value of              *
*     the CRESET signal on the I2C Bus					     *
*                                                            *
*     NOTE: This function should be modified in an embedded  *
*     system!                                                *
*                                                            *
*************************************************************/
int ToggleTRST(int toggle)
{
	//signalTRST =  toggle;
	//SetI2CDelay(1);
	return 0;
}
/*************************************************************
*                                                            *
* SetI2CStartCondition                                       *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     int                                                   *
*                                                            *
* DESCRIPTION:                                               *
*     This function is used to issue a start sequence on the *
*     I2C Bus								                 *
*                                                            *
*     NOTE: This function should be modified in an embedded  *
*     system!                                                *
*                                                            *
*************************************************************/
int SetI2CStartCondition()
{
	//SCL SDA high
	//signalSDA = 1;
	//SetI2CDelay(1);
	//signalSCL = 1;
	//SetI2CDelay(1);
	//SCL high SDA low
	//signalSDA = 0;
	//SetI2CDelay(1);
	//SCL low SDA low
	//signalSCL = 0;
	//SetI2CDelay(1);
	
	return 0;
}
/*************************************************************
*                                                            *
* SetI2CReStartCondition                                     *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     None                                                   *
*                                                            *
* DESCRIPTION:                                               *
*     This function is used to issue a start sequence on the *
*     I2C Bus								                 *
*                                                            *
*     NOTE: This function should be modified in an embedded  *
*     system!                                                *
*                                                            *
*************************************************************/
int SetI2CReStartCondition()
{
	//SCL SDA high
	//signalSDA = 1;
	//SetI2CDelay(1);
	//signalSCL = 1;
	//SetI2CDelay(1);
	//SCL high SDA low
	//signalSDA = 0;
	//SetI2CDelay(1);
	//SCL low SDA low
	//signalSCL = 0;
	//SetI2CDelay(1);
	return 0;
	
}
/*************************************************************
*                                                            *
* SetI2CStopCondition                                        *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     None                                                   *
*                                                            *
* DESCRIPTION:                                               *
*     This function is used to issue a stop sequence on the  *
*     I2C Bus								                 *
*                                                            *
*     NOTE: This function should be modified in an embedded  *
*     system!                                                *
*                                                            *
*************************************************************/
int SetI2CStopCondition()
{
	//SCL high SDA low
	//signalSDA = 0;
	//SetI2CDelay(1);
	//signalSCL = 1;
	//SetI2CDelay(1);
	//SCL high SDA high
	//signalSDA = 1;
	//SetI2CDelay(1);
	return 0;
}

/*************************************************************
*                                                            *
* READBYTESANDSENDNACK                                         *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     Returns the bit read back from the device.             *
*                                                            *
* DESCRIPTION:                                               *
*     This function is used to read the TDO pin from the     *
*     input port.                                            *
*                                                            *
*     NOTE: This function should be modified in an embedded  *
*     system!                                                *
*                                                            *
*************************************************************/
int ReadBytesAndSendNACK( int length, unsigned char *a_ByteRead , int NAck)
{	
	/*
	int i,j;
	BYTE ByteRead = 0;
	for (i = 0; i< (length+7)/8; i++)
	{
		ByteRead = 0;
		signalSDA = 1;
		for (j = 0; j < 8; j++)
		{
			ByteRead <<= 1;
			do {
				signalSCL = 1;
			} while(signalSCL_IN == 0); // wait for any SCL clock stretching
			if(signalSDA_IN)
				ByteRead |= 1;
			signalSCL = 0;
		}
		a_ByteRead[i] = ByteRead;
		if(i<((length+7)/8) -1) // keep sending Ack for "((length+7)/8)-1)" bytes
		{
			signalSDA = 0;
			signalSCL = 1;
			SetI2CDelay(1);
			signalSCL = 0;
		}
	}
	// This is while reading the last byte. If NAck = 1, send a NAck else send an Ack
	// When reading multiple pages of data, it always Ack even for the last byte
	if( NAck )
		signalSDA = 1; // send a NAck
	else
		signalSDA = 0; // send a Ack
	signalSCL = 1;
	SetI2CDelay(1);  
	signalSCL = 0;
	signalSDA = 1;
	*/
	return 0;
}
/*************************************************************
*                                                            *
* SENDBYTESANDCHECKACK                                        *
*                                                            *
* INPUT:                                                     *
*                                                            *
*     a_bByteSend: the value to determine of the pin above   *
*     will be written out or not.                            *
*                                                            *
* RETURN:                                                    *
*     true or false.                                         *
*                                                            *
* DESCRIPTION:                                               *
*     To apply the specified value to the pins indicated.    *
*     This routine will likely be modified for specific      *
*                                                            *
*     NOTE: This function should be modified in an embedded  *
*     system!                                                *
*                                                            *
*************************************************************/
int SendBytesAndCheckACK(int length, unsigned char *a_bByteSend)
{
	/*
	int i,j;
	BYTE ByteWrite = 0;
	for (i = 0; i< (length+7)/8; i++)
	{
	    ByteWrite = a_bByteSend[i];
		for (j = 0; j < 8; j++)
		{
			if( ByteWrite & 0x80 )
				signalSDA = 1;
			else 
				signalSDA = 0;
			signalSCL = 1;
			signalSCL = 0;
			ByteWrite <<= 1;			
		}
		signalSDA = 1;
		signalSCL = 1;
		SetI2CDelay(1);
		if( signalSDA_IN == 1)
		{
			signalSCL = 0;
			//fail to get ACK
			return -1;		
		}
		signalSCL = 0;
	}
	*/
	return 0;
}

/*************************************************************
*                                                            *
* SetI2CDelay                                                *
*                                                            *
* INPUT:                                                     *
*     a_uiDelay: number of waiting time.                     *
*                                                            *
* RETURN:                                                    *
*     None.                                                  *
*                                                            *
* DESCRIPTION:                                               *
*     Users must devise their own timing procedures to       *
*     ensure the specified minimum delay is observed when    *
*     using different platform.  The timing function used    *
*     here is for PC only by hocking the clock chip.         *
*                                                            *
*     NOTE: This function should be modified in an embedded  *
*     system!                                                *
*                                                            *
*************************************************************/
void SetI2CDelay( unsigned int a_msTimeDelay )
{	
	unsigned short loop_index     = 0;
	unsigned short ms_index       = 0;
	unsigned short us_index       = 0;

	/*Users can replace the following section of code by their own*/
	for( ms_index = 0; ms_index < a_msTimeDelay; ms_index++)
	{
		/*Loop 1000 times to produce the milliseconds delay*/
		for (us_index = 0; us_index < 1000; us_index++)
		{ /*each loop should delay for 1 microsecond or more.*/
			loop_index = 0;
			do {
				/*The NOP fakes the optimizer out so that it doesn't toss out the loop code entirely*/
				__asm NOP
			}while (loop_index++ < ((g_usCpu_Frequency/8)+(+ ((g_usCpu_Frequency % 8) ? 1 : 0))));/*use do loop to force at least one loop*/
		}
	}
}
/*************************************************************
*                                                            *
* ENABLEHARDWARE                                             *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     None.                                                  *
*                                                            *
* DESCRIPTION:                                               *
*     This function is called to enable the hardware.        *
*                                                            *
*     NOTE: This function should be modified in an embedded  *
*     system!                                                *
*                                                            *
*************************************************************/

void EnableHardware()
{
	SetI2CStartCondition();
	SetI2CStopCondition();
}

/*************************************************************
*                                                            *
* DISABLEHARDWARE                                            *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     None.                                                  *
*                                                            *
* DESCRIPTION:                                               *
*     This function is called to disable the hardware.       *
*                                                            *
*     NOTE: This function should be modified in an embedded  *
*     system!                                                *
*                                                            *
*************************************************************/

void DisableHardware()
{
	SetI2CStopCondition();
}

