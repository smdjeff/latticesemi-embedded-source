/***************************************************************
*
* Holds the current byte when writing to the device. Each
* bit represents TDI, TDO, TMS, TCK, etc, respectively.
*
***************************************************************/

unsigned short g_siIspPins = 0x0000;

/***************************************************************
*
* Stores the current input port address.
*
***************************************************************/

unsigned short g_usInPort = 0x379;

/***************************************************************
*
* Stores the current output port address.
*
***************************************************************/

unsigned short g_usOutPort = 0x378;

/***************************************************************
*
* This is the definition of the bit locations of each respective
* signal in the global variable g_siIspPins.
*
* NOTE: users must add their own implementation here to define
*       the bit location of the signal to target their hardware.
*       The example below is for the Lattice download cable on
*       on the parallel port.
*
***************************************************************/

const unsigned char g_ucPinTDI		= 0x01;    /* Bit address of TDI signal */
const unsigned char g_ucPinTCK		= 0x02;    /* Bit address of TCK signal */
const unsigned char g_ucPinTMS		= 0x04;    /* Bit address of TMS signal */
const unsigned char g_ucPinENABLE	= 0x08;    /* Bit address of chip enable */
const unsigned char g_ucPinTRST		= 0x10;    /* Bit address of TRST signal */
const unsigned char g_ucPinCE		= 0x20;    /* Bit address of buffer chip enable */
const unsigned char g_ucPinTDO		= 0x40;    /* Bit address of TDO signal */

/***************************************************************
*
* External variables and functions.
*
***************************************************************/

extern unsigned short g_usDelayPercent;

/***************************************************************
*
* writePort
*
* To apply the specified value to the pins indicated. This routine will
* likely be modified for specific systems. As an example, this code
* is for the PC, as described below.
*
* This routine uses the IBM-PC standard Parallel port, along with the
* schematic shown in Lattice documentation, to apply the signals to the
* programming loop. 
*
* PC Parallel port pin    Signal name   Port bit address
*  --------------------    -----------   ------------------
*        2                   g_ucPinTDI        1
*        3                   g_ucPinTCK        2
*        4                   g_ucPinTMS        4
*        5                   g_ucPinENABLE     8
*        6                   g_ucPinTRST       16
*        7                   g_ucPinCE	       32
*        10                  g_ucPinTDO        64
*                     
*  Parameters:
*   - a_ucPins, which is actually a set of bit flags (defined above)
*     that correspond to the bits of the data port. Each of the I/O port
*     bits that drives an isp programming pin is assigned a flag 
*     (through a #define) corresponding to the signal it drives. To 
*     change the value of more than one pin at once, the flags are added 
*     together, much like file access flags are.
*
*     The bit flags are only set if the pin is to be changed. Bits that 
*     do not have their flags set do not have their levels changed. The 
*     state of the port is always manintained in the static global 
*     variable g_siIspPins, so that each pin can be addressed individually 
*     without disturbing the others.
*
*   - a_ucValue, which is either HIGH (0x01 ) or LOW (0x00 ). Only these two
*     values are valid. Any non-zero number sets the pin(s) high.
*
***************************************************************/

void writePort( unsigned char a_ucPins, unsigned char a_ucValue )
{
	a_ucPins = 0;
	a_ucValue = 0;
	/***************************************************************
	*
	* Converting VME to HEX does not require hardware.
	*
	***************************************************************/

	return;
}

/***************************************************************
*
* readPort
*
* Returns the value of the TDO from the device.
*
***************************************************************/

unsigned char readPort()
{
	/***************************************************************
	*
	* Converting VME to HEX does not require hardware.
	*
	***************************************************************/

	return 0;
} 

/***************************************************************
*
* ispVMDelay
*
* Users must devise their own timing procedure to ensure the 
* specified minimum delay is observed when using different
* platform. The timing function used here is for PC only by
* hocking the clock chip. 
*
***************************************************************/

void ispVMDelay( unsigned short a_usMicroSecondDelay )
{
	a_usMicroSecondDelay = 0;
	/***************************************************************
	*
	* Converting VME to HEX does not require hardware.
	*
	***************************************************************/

	return;
}