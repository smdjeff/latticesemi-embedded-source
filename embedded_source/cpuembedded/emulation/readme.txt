                       Lattice Semiconductor Corp.
                            CPU Embedded V2.0
                              Release Notes

CPU Embedded v2.0 09-05-07
========================
UPDATE
1   Added new CPU_COMMAND_VERIFY_REGISTER (0x0E) opcode to support SRAM and XSRAM verify mode.  
    This will enable  CPU Embedded to compare the actual TDO byte data and the exepect TDO byte 
    data with the MASK byte data .  

CPU Embedded v2.0 12-6-06
========================
UPDATE
1   Added new CPU_COMMAND_BIT_DATA (0x0D) opcode to support serial programming mode.  
    This will enable  CPU Embedded to output data on the din pin, one bit at a time.  
2   The CPU simulate codes for parallel programming mode (PCM) now located in the new PCM directory.
    The CPU simulate codes for serial programming mode (SCM) now located in the new SCM directory.

CPU Embedded v2.0 7-6-06
========================
UPDATE
1   Added support for compressed embedded bitstream file.
	Included new compression opcode - CPU_COMMAND_COMPRESS(0x0C)
2   Moved all function declarations under a common header file
	Added file cpu_head.h
3   Added get() and set() functions to eliminate the use of externs when passing variables between files.
4   Renamed looping variables.
5   All functions that previously returned void now return an integer;

CPU Embedded V1.2 4-27-06
========================
UPDATE
1   Supported simulate the processing of the CPU file
    by priniting out to the monitor the CPU vector file seen by the device.
    1,0 means drive to the device pin a High or Low signal respectively.
    H,L means read from the device pin a High or Low signal respectively.
    X means read back an unknown value.
    N means the pin is not driven by the cpu file.
    z means the pin is not driven to prepare for bus turn-around for reading. 
    ? means the pin is being tested for looping.
    Wait xxxxus or xxxxms means wait for that time for xxxx micro-seconds or millie-seconds.
    Loop XXXX means perform the following loop xxxx amount of time.

CPU Embedded V1.2 3-28-06
========================
UPDATE
1	Supported FLASH and XFLASH CPU Programming.  
2	Supported "Erase,Program,Verify", "Erase,Program,Verify and Secure", "Erase Only", "Verify Only" and "Verify ID" Operations.
3   Added CPU_COMMAND_READ_REGISTER(0x0A) opcode to support Read and Verify
4   Added CPU_COMMAND_HIGH_SPEED_CLK(0x0B) opcode to support the required fast CCLK.


CPU Embedded V1.1 8-9-04
========================
UPDATE
1	CFG pins support.  The embedded bitstreams may be generated with commands to set the CFG pins.  This allows
    the user to specify the programming mode (Master PCM, Slave PCM, etc).  By default, the CFG pins shall not
    be specified.  The CPU Embedded source code has been updated accordingly to process commands that drive the
    CFG pins.
2	BUSY pin support.  The embedded bitstreams may be generated with commands to check the BUSY pin.  This is
    required to program FLASH devices.  The CPU Embedded source code has been updated accordingly to process
    commands that check the BUSY pin.

