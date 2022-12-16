# hamster
A small but clever single board 8bit micro computer built around attiny series 2 chip

## useful
- https://www.microchip.com/en-us/product/ATtiny1626
- example codes in github: https://github.com/search?p=2&q=ATtiny1627+user%3Amicrochiptech+user%3Amicrochip-pic-avr-examples&type=Repositories
- bootloader for tinyAvr0- and 1- series (we have 2- series, but should be similar)
https://www.microchip.com/en-us/application-notes/an2634



## Development environment
- We use avr-gcc with attiny1626 chip-support. You can get this from: 
   - installing the latest Arduino IDE, or
   - get GNU AVR GCC toolchain and device files from Atmel. (https://www.microchip.com/en-us/tools-resources/develop/microchip-studio/gcc-compilers, http://packs.download.atmel.com/)   
- The pymcuprog library: https://github.com/microchip-pic-avr-tools/pymcuprog (we will use Serial port UPDI feature)
- We need a programmer to debug the chip using UPDI and to install the kernel to the chip
  - I used a cheeap CH340G based USB-to-serial board, adding a single schottky diode between Rx -->|-- Tx (see the wiring here): https://github.com/SpenceKonde/AVR-Guidance/blob/master/UPDI/jtag2updi.md (you may need to download driver for the CH340-based serial adapter, although it was not needed for me on MacOS Big Sur, it recognised without driver: https://learn.sparkfun.com/tutorials/how-to-install-ch340-drivers/all )
  - Alternatively you can also use the cheap MPLab SNAP board (https://www.microchip.com/en-us/development-tool/PG164100) withe some necessary modifications (https://ww1.microchip.com/downloads/en/DeviceDoc/ETN36_MPLAB%20Snap%20AVR%20Interface%20Modification.pdf)
  - Or use any more sophisticated professional UPDI AVR programmer


Prepare gcc toolchain based on GNU AVR GCC toolchain:
```
mkdir ~/avr-toolchain
cd ~/avr-toolchain

tar xzvf ~/git/hamster/toolchain/avr8-gnu-toolchain-3.7.0.1796-linux.any.x86_64.tar.gz
unzip ~/git/hamster/toolchain/Atmel.ATtiny_DFP.2.0.368.atpack -d ./avr8-attiny-chips
```

Prepare virtualenv:
```
pip install virtualenv
virtualenv hamster_pyenv -p python3
source hamster_pyenv/bin/activate
pip3 install pymcuprog
```

Make sure you have permission to use the usb serial device
```
ls -la /dev/ttyUSB0
crw-rw---- 1 root dialout 188, 0 szept 23 18:41 /dev/ttyUSB0

 sudo usermod -a -G dialout `whoami`
```

### Setting up the development environment
If you use Visual Studio Code, then install "C/C++ Extension Pack" and set your project specific `.vscode/c_cpp_properties.json` file to something like this:
```
{
    "configurations": [
        {
            "name": "AVR",
            "includePath": [
                "/home/mate/avr-toolchain/avr8-attiny-chips/include/",
                "/home/mate/avr-toolchain/avr8-gnu-toolchain-linux_x86_64/avr/include/avr/**",
                "/home/mate/avr-toolchain/avr8-gnu-toolchain-linux_x86_64/avr/include/**",
                "${workspaceFolder}/**"
            ],
            "defines": [
                "__AVR_ATtiny1626__",
                "ATtiny1626"
            ],
            "compilerPath": "/home/mate/avr-toolchain/avr8-gnu-toolchain-linux_x86_64/bin/avr-gcc",
            "cStandard": "c99",
            "intelliSenseMode": "gcc-x86",
            "compilerArgs": [
               "-mmcu=attiny1626",
               "-B /home/mate/avr-toolchain/avr8-attiny-chips/gcc/dev/attiny1626/"
            ]
        }
    ],
    "version": 4
}
```

### check programmer connection (first serial programmer, second using MPLAB Snap)
```
 > source cricom_pyenv/bin/activate   
 
 > pymcuprog ping -t uart -d attiny1626 -u /dev/cu.usbserial-14340
 Pinging device...
 Ping response: 1E9429
 Done.

> pymcuprog ping -d attiny1626 -t snap -s BUR204482722
Connecting to snap (BUR204482722)'
Connected to MPLAB Snap ICD CMSIS-DAP from Microchip Technology Incorporated (serial number BUR204482722)
Debugger firmware version 1.10.249
Debugger hardware revision 1
Pinging device...
Ping response: 1E9429
Done.
```

### compile the kernel and deploy it to the board
```
rebuild.sh
```



### complie bootloader (using XC8 compiler)
you might need to include xc.h in the kernel

```
cd bootloader
# elf file is for debugging?
/Applications/microchip/xc8/v2.35/bin/xc8-cc -mcpu=attiny1626 -O2 -o bootloader/hello.elf bootloader/bootloader.c
# hex file for upload
/Applications/microchip/xc8/v2.35/bin/avr-objcopy -O ihex bootloader/hello.elf bootloader/hello.hex
```

### manually upload to the board
```
pymcuprog write -t uart -d attiny1626 -u /dev/cu.usbserial-14340 -f bootloader/hello.hex

pymcuprog -t uart -d attiny1626 -u /dev/cu.usbserial-14340 -v debug --chip-erase-locked-device erase

pymcuprog  -d attiny1626 -t snap -s BUR204482722 ping
pymcuprog  -d attiny1626 -t snap -s BUR204482722 write -f bootloader/hello.hex
```
