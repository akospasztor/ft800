FT800
=====
http://www.ftdichip.com/Products/ICs/FT800.html

C Library for FTDI FT800 EVE controller.
- Easy to use
- Flexible
- Can be easily ported to other microcontrollers
- Tested with ARM Cortex-M4 microcontroller (STM32F4xx series)

## Usage
Functions and defines can be found in ft800.h and ft800.c files. No modifications should be made in these files.

The low-level functions can be found in spi.c. These functions should be modified, depending on the pinout and your microcontroller.

The library can be used with STM32F4 Discovery without any modifications. Just connect the wires to proper pins:
- SCK  = PA5
- MISO = PA6
- MOSI = PB5
- CS   = PA4
- PDN  = PE8

## Functions
The following functions are implemented in this library. These functions are enough to implement complex user interfaces with touch control.

### Host functions
- HOST_CMD_ACTIVE    //send wake-up command
- HOST_CMD_WRITE     //send host command
- HOST_MEM_READ_STR  //read string from memory
- HOST_MEM_WR_STR    //write string into memory
- HOST_MEM_WR8       //write 1byte data into memory
- HOST_MEM_WR16      //write 2byte data into memory
- HOST_MEM_WR32      //write 4byte data into memory
- HOST_MEM_RD8       //read 1byte from memory
- HOST_MEM_RD16      //read 2byte from memory
- HOST_MEM_RD32      //read 4byte from memory

### Co-processor functions
- cmd                //command function
- cmd_ready          //check if co-proc. is ready
- cmd_track          //set tracking
- cmd_spinner        //draw spinner
- cmd_slider         //draw slider
- cmd_text           //draw text
- cmd_button         //draw button
- cmd_keys           //draw keyboard
- cmd_memzero        //write zero to a block of memory
- cmd_fgcolor        //set foreground color
- cmd_bgcolor        //set background color
- cmd_gradcolor      //set gradient color
- cmd_gradient       //draw gradient
- cmd_loadidentity   //set current matrix to identity matrix
- cmd_setmatrix      //write current matrix to the display list
- cmd_rotate         //apply rotation to current matrix
- cmd_translate      //apply translation to current matrix


## Examples
An initialization example for a 5” display, and a demo screen example can be found in main.c.

Please note that a millisec delay function should be implemented in order to use these examples.

Watch the FT800 in action with an 5” LCD touch screen in my Home Automation project: http://akospasztor.com/projects/homecontrol/