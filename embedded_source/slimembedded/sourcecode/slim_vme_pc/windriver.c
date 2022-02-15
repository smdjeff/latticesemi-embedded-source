#include "opcode.h"

#ifdef VME_WINDOWS
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#include <conio.h>
#include "windriver.h"

/*************************************************************
*                                                            *
* EXTERNAL FUNCTIONS                                         *
*                                                            *
*************************************************************/

extern void writePort( unsigned char a_ucPins, unsigned char a_ucValue );
extern void ispVMDelay( unsigned int a_uiDelay );

/*************************************************************
*                                                            *
* EXTERNAL VARIABLES                                         *
*                                                            *
*************************************************************/

extern short int g_siOutPort;
extern short int g_siInPort;

/*************************************************************
*                                                            *
* GET_PORT_ADDRESSES                                         *
*                                                            *
* INPUT:                                                     *
*     a_iPort: the port to get the address.                  *
*                                                            *
*     a_piInputAddress: pointer to hold the input address.   *
*                                                            *
*     a_piOutputAddress: pointer to hold the output address. *
*                                                            *
* RETURN:                                                    *
*     None.                                                  *
*                                                            *
* DESCRIPTION:                                               *
*     This function gets the possible addresses on the       *
*     parallel port.                                         *
*     This function is tailored to the Windows platform and  *
*     must be overwritten if user is targeting another       *
*     platform.  It is not required to be overwrriten.       *
*                                                            *
*************************************************************/

void get_port_addresses( int a_iPort, short int * a_piInputAddress, short int * a_piOutputAddress )
{
    switch ( a_iPort ) { 
	case 1:
		*a_piOutputAddress = 0x0378;
		*a_piInputAddress = 0x0379;
		break;
	case 2:
		*a_piOutputAddress = 0x0278;
		*a_piInputAddress = 0x0279;
		break;
	case 3:
		*a_piOutputAddress = 0x03BC;
		*a_piInputAddress = 0x03BD;
		break;
	default:
		*a_piOutputAddress = 0;
		*a_piInputAddress = 1;
		break;
    } 
}

/*************************************************************
*                                                            *
* POWER_OK                                                   *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     Returns a positive number if passing, or 0 if failure. *
*                                                            *
* DESCRIPTION:                                               *
*     This function checks to see if the parallel port has   *
*     power.                                                 *
*     This function is tailored to the Windows platform and  *
*     must be overwritten if user is targeting another       *
*     platform.  It is not required to be overwrriten.       *
*                                                            *
*************************************************************/

int power_ok()
{
	int iRetCode, iPrevRetCode;
	
	writePort( pinENABLE, 0x00 );
	writePort( pinCE, 0x01 );
	writePort( pinTCK + pinTMS + pinTDI + pinTRST, 0x00 );

	ispVMDelay( 5 );
	iPrevRetCode = ( IN_VCC_OK & _inp( g_siInPort ) );

	ispVMDelay( 5 );
	iRetCode = ( IN_VCC_OK & _inp( g_siInPort ) );

	if ( iPrevRetCode != iRetCode ) {
		iRetCode = 0;
	}
	return iRetCode;
}

/*************************************************************
*                                                            *
* PORT_OK                                                    *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     Returns true if a cable was found connected, otherwise *
*     returns false.                                         *
*                                                            *
* DESCRIPTION:                                               *
*     This function checks to see if the cable is connected  *
*     to the specified port.                                 *
*     This function is tailored to the Windows platform and  *
*     must be overwritten if user is targeting another       *
*     platform.  It is not required to be overwrriten.       *
*                                                            *
*************************************************************/

int port_ok()
{
	int d1, d2;     
	short int portData = 0;

	writePort( pinENABLE, 0x00 ); 
	writePort( pinCE, 0x01 );
	writePort( OUT_SENSE_CABLE_OUT, 0x01 );

	ispVMDelay( 10 );
	d1 = ( IN_CABLE_SENSE_IN & _inp( g_siInPort ) );

	writePort( OUT_SENSE_CABLE_OUT, 0x00 );
	
	ispVMDelay( 10 );
	d2 = ( IN_CABLE_SENSE_IN & _inp( g_siInPort ) );
	
	if ( d2 != d1 ) {
		return 1;
	}
	else {
		return 0;
	}
}

/*************************************************************
*                                                            *
* ANYCABLESCONNECTED                                         *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     Returns true if a cable was found connected, otherwise *
*     returns false.                                         *
*                                                            *
* DESCRIPTION:                                               *
*     This function checks to see if the cable is connected. *
*     This function is tailored to the Windows platform and  *
*     must be overwritten if user is targeting another       *
*     platform.  It is not required to be overwrriten.       *
*                                                            *
*************************************************************/

int AnyCablesConnected()
{ 
	int lptnum;
    short int inport, outport;
    int flag = FALSE;  
    inport = g_siInPort;
    outport = g_siOutPort;
    for (lptnum = 1; lptnum <= 3; lptnum++)
	{
		get_port_addresses(lptnum, &g_siInPort, &g_siOutPort);
		if (port_ok()){
			flag = TRUE;
		}
    }       
    g_siInPort = inport;
    g_siOutPort = outport;  
    return (flag);
}

/*************************************************************
*                                                            *
* CABLE_PORT_CHECK                                           *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     Returns 1 if passing, or 0 if failure.                 *
*                                                            *
* DESCRIPTION:                                               *
*     This function checks to see if a Lattice download      *
*     cable is connected.                                    *
*                                                            *
*************************************************************/

int cable_port_check()
{
    unsigned int d1, d2;
    short int  portData = 0;
    int success = FALSE;     
	int old_cable = 0;
	/*4/21/99 Nguyen added to fix the problem of JAM Player*/
	if (g_siOutPort ==0x03BC)
		_outp(0x03BE, 0x00);
	else if (g_siOutPort == 0x0378) 
		_outp(0x037A, 0x00);
	else if (g_siOutPort == 0x0278) 
		_outp(0x027A, 0x00);
	else
		_outp(0x037A, 0x00);
	writePort(OUT_SENSE_CABLE_OUT, 0x00);
    ispVMDelay(10);
    d1 =((IN_CABLE_V2_SENSE_IN + IN_CABLE_SENSE_IN) & _inp(g_siInPort));
    writePort(OUT_SENSE_CABLE_OUT, 0x01);
    ispVMDelay(10);
    d2 =((IN_CABLE_V2_SENSE_IN + IN_CABLE_SENSE_IN) & _inp(g_siInPort));
    if (d2 == IN_CABLE_V2_SENSE_IN)
		old_cable = 0;
    else
		old_cable++;
    /* d2 and d1 will be equal if the cable is not installed */
    if (d2 != d1){
		/* V2.0 cable exist?*/
		writePort(OUT_SENSE_CABLE_OUT, 0x00);
		ispVMDelay(1);
		d1 =(IN_CABLE_V2_SENSE_IN & _inp(g_siInPort));
		writePort(OUT_SENSE_CABLE_OUT, 0x01);
		ispVMDelay(1);
		d2 =(IN_CABLE_V2_SENSE_IN & _inp(g_siInPort));
		if (d2 != d1)
			old_cable = 0;
		else
			old_cable++;
		success = TRUE;
    }         
    return (success);
}

/*************************************************************
*                                                            *
* GETPORTADDR                                                *
*                                                            *
* INPUT:                                                     *
*     a_iPortIndex: port index.                              *
*                                                            *
* RETURN:                                                    *
*     Returns 0 if passing, or negative number if failure.   *
*                                                            *
* DESCRIPTION:                                               *
*                                                            *
*************************************************************/

int get_possible_ports( int a_iPortIndex )
{
    int valid_port = FALSE;
    BOOL next_tests = TRUE;

	get_port_addresses( a_iPortIndex, &g_siInPort, &g_siOutPort );
	if ( g_siOutPort == 0x00 ) {
		next_tests = FALSE;  
		return ( 0 );
	}
	if ( next_tests ){
		get_port_addresses( a_iPortIndex, &g_siInPort, &g_siOutPort );
	}
    return ( 1 );
}

/*************************************************************
*                                                            *
* GETPORTADDR                                                *
*                                                            *
* INPUT:                                                     *
*     a_psiInport: input address.                            *
*                                                            *
*     a_psiOutport: output address.                          *
*                                                            *
* RETURN:                                                    *
*     Returns 0 if passing, or negative number if failure.   *
*                                                            *
* DESCRIPTION:                                               *
*                                                            *
*************************************************************/

int GetPortAddr( short int * a_psiInport, short int * a_psiOutput )
{ 
	int rcode, rcode2 = 0;
	int lptnum;    
    for ( lptnum = 1; lptnum <= 3; lptnum++ ) {
		rcode = get_possible_ports( lptnum );
		if ( rcode == 1 ) {
			rcode2 = 0;
			break;
		}
		else if ( rcode == 2 ) {
			rcode2 = POWER_OFF;
		}
    }  
	get_port_addresses( lptnum, a_psiInport, a_psiOutput );
	return ( rcode2 );
}

/*************************************************************
*                                                            *
* CHECK_CABLE_POWER                                          *
*                                                            *
* INPUT:                                                     *
*     None.                                                  *
*                                                            *
* RETURN:                                                    *
*     Returns 0 if passing, or negative number if failure.   *
*                                                            *
* DESCRIPTION:                                               *
*     This function checks to see if the cable has power.    *
*     This function is tailored to the Windows platform and  *
*     must be overwritten if user is targeting another       *
*     platform.  It is not required to be overwrriten.       *
*                                                            *
*************************************************************/

int Check_Cable_Power()
{
	int iRetCode = 0;
	
	if ( g_siOutPort == 0x03BC ) {
		_outp( 0x03BE, 0x00 );
	}
	else if ( g_siOutPort == 0x0378 ) {
		_outp( 0x037A, 0x00 );
	}
	else if ( g_siOutPort == 0x0278 ) {
		_outp( 0x027A, 0x00 );
	}
	else {
		_outp( 0x037A, 0x00 );
	}

	if ( !AnyCablesConnected() ) {
		return( CABLE_NOT_DETECT );
	}

	iRetCode = GetPortAddr( &g_siInPort, &g_siOutPort ); 
	if ( iRetCode == POWER_OFF ) {
		if ( g_siOutPort ==0 ){
			return( CABLE_NOT_DETECT );
		}
		if ( !power_ok() ) {
			return( POWER_OFF );
		}
	}
	return(0);
}

#endif /* VME_WINDOWS */