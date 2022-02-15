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

/*************************************************************************
*
* Embedded bitstream command opcodes.
*
* 0x01 - 0x4F are reserved for command opcodes(79 maximum).
*
**************************************************************************/

#define CPU_COMMAND_COMMENT				0x01
#define CPU_COMMAND_BYTE_DATA			0x02
#define CPU_COMMAND_VERSION				0x03
#define CPU_COMMAND_END_BYTE			0x04
#define CPU_COMMAND_SET_PIN				0x05
#define CPU_COMMAND_CHECK_PIN			0x06
#define CPU_COMMAND_SEND_REGISTER		0x07
#define CPU_COMMAND_CRC					0x08
#define CPU_COMMAND_SET_CFG_REGISTER	0x09
#define CPU_COMMAND_READ_REGISTER		0x0A
#define CPU_COMMAND_HIGH_SPEED_CLK		0x0B
#define CPU_COMMAND_COMPRESS			0x0C
#define CPU_COMMAND_BIT_DATA			0x0D
#define CPU_COMMAND_VERIFY_REGISTER		0x0E

/*************************************************************************
*
* Embedded bitstream pin opcodes.
*
* 0x50 - 0x7F are reserved for command opcodes(47 maximum).
*
**************************************************************************/

#define CPU_PROG_PIN					0x50
#define CPU_INIT_PIN					0x51
#define CPU_DONE_PIN					0x52
#define CPU_CS_PIN						0x53
#define CPU_CS1_PIN						0x54
#define CPU_WRITE_EN_PIN				0x55
#define CPU_CCLK_PIN					0x56
#define CPU_D0_PIN						0x57
#define CPU_D1_PIN						0x58
#define CPU_D2_PIN						0x59
#define CPU_D3_PIN						0x5A
#define CPU_D4_PIN						0x5B
#define CPU_D5_PIN						0x5C
#define CPU_D6_PIN						0x5D
#define CPU_D7_PIN						0x5E
#define CPU_BUSY_PIN					0x5F
#define CPU_DIN_PIN						0x57

/*************************************************************************
*
* Embedded bitstream pin value opcodes.
*
**************************************************************************/

#define CPU_LOW							0x00
#define CPU_HIGH						0x01

/*************************************************************************
*
* Embedded bitstream return codes.
*
**************************************************************************/

#define CPU_SUCCESS						0
#define CPU_INSUFFICIENT_ARGUMENTS		-1
#define CPU_FILE_NOT_FOUND				-2
#define CPU_INVALID_FILE				-3
#define CPU_VERIFICATION_FAILURE		-4
#define CPU_INVALID_VERSION				-5
#define CPU_CRC_FAILURE					-6
#define CPU_OUT_MEMORY					-7

/*************************************************************************
*
* Embedded bitstream version information.
*
**************************************************************************/

#define CPU_VERSION						"2.0.0"
#define _MAX_VERSION_LENGTH_			7

