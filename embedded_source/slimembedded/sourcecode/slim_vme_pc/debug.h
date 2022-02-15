/************************************************************************
*						      DEBUG.H									*
*																		*
*		This file is used for debugging purposes only.  The array       *
*		ispVMOpcodes[] is printed out to let the user know which        *
*		opcode failed.													*
*																		*
*                                                                       *
*************************************************************************/

static struct ispVMOpcode
{
	const char * a_pszText;
	unsigned char ucOpcode;
} ispVMOpcodes[] = 
{
	{ "ENDVME",	ENDVME },
	{ "STATE",	STATE },
	{ "SIR", SIR },
	{ "SDR", SDR },
	{ "TCK", TCK },
	{ "WAIT", WAIT },
	{ "ENDDR", ENDDR },
	{ "ENDIR", ENDIR },
	{ "HIR", HIR },
	{ "TIR", TIR },
	{ "TDR", TDR },
	{ "HDR", HDR },
	{ "BEGIN_REPEAT", BEGIN_REPEAT },
	{ "FREQUENCY", FREQUENCY },
	{ "TDI", TDI },
	{ "CONTINUE", CONTINUE },
	{ "END_FRAME", END_FRAME },
	{ "TDO", TDO },
	{ "MASK", MASK },
	{ "END_REPEAT", END_REPEAT },
	{ "DATA", DATA },
	{ "PROGRAM", PROGRAM },
	{ "VERIFY", VERIFY },
	{ "LCOUNT", LCOUNT },
	{ "LDELAY", LDELAY },
	{ "LSDR", LSDR },
	{ "LOOP", LOOP },
	{ "ENDLOOP", ENDLOOP },
};

int iOpcodeCount = sizeof( ispVMOpcodes ) / sizeof( ispVMOpcodes[ 0 ] );

