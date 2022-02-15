#ifndef cpu_head_h_ 
#define cpu_head_h_ 
/**************************************************************/
/*
/* Lattice Semiconductor Corp.
/* 
/* CPU Embedded allows programming of Lattice's suite of FPGA
/* devices on embedded systems through the sysCONFIG port.  The software
/* is distributed in source code form and is open to re - distribution
/* and modification where applicable.
/*
/**************************************************************/

/**************************************************************
*
* This file contains the function declarations for 
* for the entire program.  They have been placed into this 
* one file for easier accesibility, as well as to eliminate the
* need to call extern in each of the files.
*
**************************************************************/


/**************************************************************
*
* main_ .c function declarations
*
**************************************************************/

char * error_handler(signed char a_cRetCode);
unsigned char get_byte();
int storeBitstreamPointer();
int restoreBitstreamPointer();
unsigned long getCurrentbyteIndex();
int setCurrentbyteIndex(unsigned long byteIndex);
unsigned short getUsCalculatedCRC();


/**************************************************************
*
* cpu_sim.c and cpu_hard.c function declarations
*
**************************************************************/

int out_port(unsigned short a_usData);
signed char set_pin(unsigned char a_ucPin, unsigned char a_ucPinValue);
signed char check_pin(unsigned char a_ucPin, unsigned char a_ucPinValue,
unsigned long a_ulDelay, unsigned long a_ulRepetition);
int set_byte_data(unsigned char a_ucData);
int set_cfg_register(unsigned short a_usCFGPinCount, unsigned char a_usCFGRegister);
unsigned short in_port();
unsigned short in_port_with_mask(unsigned short a_usMaskData);
int set_delay(unsigned long a_ulDelay);
unsigned short getUsRegister();



/**************************************************************
*
* cpu_core.c function declarations
*
**************************************************************/

int command_comment();
int command_byte_data();
signed char command_version();
signed char command_set_pin();
signed char command_check_pin();
signed char command_crc();
unsigned short calculate_crc(unsigned long g_ulDataSize, unsigned long g_ulCurrentByteIndex);
int command_set_cfg_register();
signed char cpu_engine();
int command_high_speed_clk();
signed char command_byte_verify_data();
signed char command_byte_verify_data_with_mask();
int command_compress();


#endif