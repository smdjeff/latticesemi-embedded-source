                       Lattice Semiconductor Corp.
                      Slim ispVM(tm) Embedded V1.0
                             Release Notes

Slim ispVM Embedded v1.4	Date: 1/11/18
=============================================
UPDATE
1	Updated the Slim VME to support compression key of 0xFF or 0x00

Slim ispVM Embedded v1.3	Date: 9/15/06
=============================================
UPDATE
1	Updated the Slim VME to support new POLING STATUS LOOP opcodes (LOOP and ENDLOOP) .
    for Flash programming of the Lattice FPGA devices
2   Added Global variables initialization
3   Typed cast all mismatch variables

Slim ispVM Embedded v1.2	Date: 7/20/06
=============================================
UPDATE
1	Updated the Slim VME for Flash programming of the LatticeXP devices.
2   Support toggling the ispENABLE and TRST pin signal.

Slim ispVM Embedded v1.1	Date: 2/20/03
=============================================
UPDATE
1	Unify Slim VME opcodes with Full VME.
2	Remove the VME_NT preprocessor flag from the released source code files.

Slim ispVM Embedded v1.0	Date: 8/18/03
=============================================
1	Description
After the successful release of Lattice Semiconductor’s IEEE 1532 compliant PLD family of devices, 
the time is appropriate to bring out the Slim ispVME to reduce the embedded programming cost.  This 
Slim ispVME is designed for 8051 micro-controller applications where RAM resources are limited.

The reduction in the required resources and the size of the footprint of the Slim ispVME is possible 
by taking advantage of the much-simplified programming algorithm adopted by Lattice Semiconductor’s 
newly released IEEE 1532 compliant PLD families:
·	MACH4000V/B/C/ZC
·	ispXPGA
·	ispXPLD
·	ispGDX2 
·	ispGAL 22V10A 
·	MACH5000B 
·	MACH5000VG 

The result is that Slim ispVME is the lowest cost embedded programming solution in the industry.

Unlike the Full VME, the VME generator utility will not be available as a separate executable.  
Instead it will be integrated into ispVM System and will only be available through the graphical 
user interface.  Another difference is that the VME file will consist of two separate files: an algorithm 
file and a data file.  Not every operation will generate a data file.  However, every operation will 
generate an algorithm file.

Once the VME data and algorithm files have been generated, it can be fed into the Slim VME Processor.  The
Slim VME Processor will require substantially less working RAM to program the devices.  In the Full VME 
version, the bulk of the working RAM usage came as a result of storing the TDI, TDO, and MASK data of 
the active frame.  This means that if the frame size of the device is 1200 bits, it could potentially 
require 3600 bits, or 450 bytes, of working RAM to process a single frame.  This also leads to another 
disadvantage of the Full VME, which is that the working RAM size is device dependent.  The new Slim VME 
Processor is designed in such a manner that the required working RAM for all devices is fixed at a minimal 
set of bytes.  This is possible in part due to the fact that IEEE 1532 devices do not require shifting in 
the frame address, and MASK data is not required.  However, the credit for the minimal set of bytes working RAM 
requirement goes to a new method of accessing the data one byte at a time from the ROM.  This must be 
carefully designed in such a way that the correct byte from the algorithm and data arrays must be accessed 
to avoid failure.

2	Limitations
To achieve the minimum resources consumption target and to bring the Slim ispVME to the smallest footprint 
possible, the following restrictions are enforced as a consequence:
·	Sequential programming only on daisy chain
·	Turbo programming is not supported
·	Only Lattice Semiconductor’s IEEE 1532 compliant devices are supported
·	Read and Save operation not supported
·	Display operations are not supported (e.g. Display USERCODE, Calculate Checksum)

3	How to use Slim ispVM Embedded
3.1	Generating Slim VME Files
The VME files can be generated through ispVM System by selecting the Slim VME button on the toolbar menu.
The Slim VME generation dialog allows the user the option of generating the file in hex or binary.  The 
binary VME file is used by the PC version of Slim ispVM Embedded and utilizes the extension *.VME.  The hex
VME file is a C programming language file that must be compiled with the 8051 version of Slim ispVM 
Embedded and utilizes the extension *.C.  By default, the binary file is generated.  Other options are 
available through the dialog, such as whether to compress the data file or disable generating the algorithm
or data file.

3.2	Processing the Slim VME Files
The binary VME files can be processed through ispVM System under the selection ispTools->Slim VME Processor.
The Process VME dialog requires a mandatory algorithm file and an optional data file.  The processing is done
over the parallel port.

3.3	Slim ispVM Embedded Source code
Both the 8051 and PC version of the Slim ispVM Embedded Processor is shipped with ispVM System.  The only
major difference between the 8051 and PC source code is the slim_vme.c file.  This file contains the main 
function which dictates how the user can invoke the processor.  The PC version expects the user to pass in 
the VME files as command line arguments, while the 8051 version expects VME hex files which are to be compiled 
into the executable.

The source code can be found in the directory <ispVM Install Path>\ispSlimVMEmbedded\SourceCode\src\.  The 
required files are:
·	slim_vme.c
·	slim_pro.c
·	hardware.c
·	opcode.h

The optional files are:
·	windriver.c
·	windriver.h
·	debug.h
·	ntdriver.c (internal Lattice use only)
·	ntdriver.h (internal Lattice use only)
·	gpioctl.h (internal Lattice use only)

3.3.1	Description of file slim_vme.c
The file slim_vme.c contains the main function and the entry point into the Slim ispVM Processor.

As mentioned above, this file is the only file between the 8051 and PC version that is different because of the way each
interfaces with the VME data.  

This file shall not be modified by the user otherwise it may compromise the processor.

3.3.2	Description of file slim_pro.c
This file contains the core of the embedded processor.  It processes the commands extracted from the VME by executing the 
appropriate functions.  The processor exits when a failure has occurred or the end of the algorithm array has been reached.

This file contains the global variables that are stored in RAM and used as indexes to access the algorithm and data arrays.
The global variables are:

int g_iMovingAlgoIndex;
int g_iMovingDataIndex;
int g_iMainDataIndex;
int g_iRepeatIndex;
int g_iTDIIndex;
int g_iTDOIndex;
int g_iMASKIndex;
unsigned char g_ucCompressCounter;
unsigned char g_cEndDR;
unsigned char g_cEndIR;
short int g_siHeadDR;
short int g_siHeadIR;
short int g_siTailDR;
short int g_siTailIR;

On a 32-bit microprocessor, an int type requires 4 bytes, a short int requires 2 bytes, and a char requires 1 byte.  The
total RAM required by these variables are fixed at 43 bytes for any chain configuration of devices, which is much less than
the requirement of the Full ispVM Embedded.  Constant global variables are excluded from this calculation because the compiler
automatically stores them into ROM.  Also note that the 43 byte RAM requirement does not include functions and function
variables that are stored on the stack.  Heap memory is not used in the 8051 version.

This file shall not be modified by the user otherwise it may compromise the processor.

3.3.3	Description of file hardware.c
This file contains the hardware-related functions of the processor.  These functions will require the users to overwrite and 
implement their own methods to communicate with the hardware.

3.3.4	Description of file opcode.h
This file contains the definitions used by the processor.  

This file shall not be modified by the user otherwise it may compromise the processor.

3.3.5	Description of windriver.h and windriver.c
This file contains the functions and definitions required to run the Slim ispVM Embedded on Windows 95 and 98.  These are
optional files and are included in the compilation if VME_WINDOWS is defined as a preprocessor switch.

3.3.6	Description of debug.h
This file contains the debugging structure to indicate which commands, or opcodes, have been processed.  This is an optional
file and is included in the compliation if VME_DEBUG is defined as a preprocessor switch.

3.4	Compiling the 8051 version
When compiling the 8051 version of Slim ispVM Embedded, the algorithm VME hex file (which is in the C program format) is 
needed to be compiled into the program.  The data VME hex file is optional, depending on the operation.  These two files 
add additional buffers that hold the VME algorithm and data files.  These buffers are constant buffers thus they will be 
stored in ROM.
