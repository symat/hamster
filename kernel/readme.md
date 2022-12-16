# Kernel code

Most basic functions, which will end up int he boot area of the flash. These routines are always present on the chip and they can not be modified by any application code (only using a programmer). In order to use them in any application or library, you need to include `kernel.h`. The kernel functions are very low level and optimized to give as much flash/ram space as possible for the applications. If any functions are not critical to be always available, then they should be part of standard libraries to be optionally linked into the programs. 

The main functions of the various kernel modules:
- `usb` module: responsible for implementing a basic serial interface where the CriCom board is acting as an USB slave. Bytes can be read or write synchronously through usb to any usb master. 
- `video` module: basic low-level functions to refresh the screen, and/or send the data through USB to an external device.
- `flash` module: low level routines to copy data between the external on-board flash memory and the mcu-internal flash banks allowing to load/save various applications and data from/to the flash. 
- `buttons` module: read the current state of the on-board push buttons, register interrupt handlers for button events. 
- `speaker` module: basic functions to play tones on the speaker.
- `power` module: basic functions to put the board into sleep mode and to wake it up on key-press. BDO configuration and barrery voltage level monitoring is also implemented here.
- `expander` module: functions needed to use the expander on the board.