#!/bin/bash

TOOLCHAIN="/home/mate/avr-toolchain/avr8-gnu-toolchain-linux_x86_64"
EXTRA_CHIP_DEFS="/home/mate/avr-toolchain/avr8-attiny-chips"
MCU="attiny824"
MAX_PROGRAM_MEMORY=$((8*1024))
MAX_RAM=$((1*1024))

PROGRAMMER_DEVICE="/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0"

BUILD_DIR=`pwd`/build
APPS_DIR=`pwd`/apps
KERNEL_DIR=`pwd`/kernel
INCLUDE_PATHS="-I${EXTRA_CHIP_DEFS}/include/ -I${TOOLCHAIN}/avr/include/avr -I${KERNEL_DIR} -I`pwd`"
GCC=${TOOLCHAIN}/bin/avr-gcc
OBJ_COPY=${TOOLCHAIN}/bin/avr-objcopy
AVR_SIZE=${TOOLCHAIN}/bin/avr-size

#CFLAGS="-g -Wall -mcall-prologues -mmcu=${MCU} -Os"
#LDFLAGS="-Wl,-gc-sections -Wl,-relax"

#CFLAGS="-Wall -Os -std=c99 -mmcu=${MCU} -fdata-sections -B ${EXTRA_CHIP_DEFS}/gcc/dev/${MCU}/"
#LDFLAGS="-mmcu=${MCU} -Wl,--gc-sections,-relax -mrelax  -B ${EXTRA_CHIP_DEFS}/gcc/dev/${MCU}/"

CFLAGS="-Wall -Werror -Os -std=c99 -mmcu=${MCU} -fdata-sections -B ${EXTRA_CHIP_DEFS}/gcc/dev/${MCU}/"
LDFLAGS="-mmcu=${MCU} -Wl,--gc-sections,-relax -mrelax  -B ${EXTRA_CHIP_DEFS}/gcc/dev/${MCU}/"

KERNEL_ELF="${BUILD_DIR}/kernel/kernel.elf"
KERNEL_HEX_FILE="${BUILD_DIR}/kernel/kernel.hex"



function build_kernel() {
  echo "building kernel code in ${KERNEL_DIR}"
  mkdir -p ${BUILD_DIR}/kernel
  ALL_OBJECT_FILES=""
  KERNEL_ELF="${BUILD_DIR}/kernel/kernel.elf"
  KERNEL_HEX_FILE="${BUILD_DIR}/kernel/kernel.hex"
  set -e
  for SOURCE_FILE in ${KERNEL_DIR}/*.c
  do
    echo "compile: ${SOURCE_FILE}"
    BASENAME=$(basename -- "$SOURCE_FILE")
    FILENAME="${BASENAME%.*}"
    OBJ_FILE=${BUILD_DIR}/kernel/$FILENAME.o
    ALL_OBJECT_FILES="${ALL_OBJECT_FILES} ${OBJ_FILE}"
    COMMAND="${GCC} -c ${CFLAGS} ${INCLUDE_PATHS} -o ${OBJ_FILE} $SOURCE_FILE"
    #echo "${COMMAND}"
    ${COMMAND}
  done
  if [ -z "${ALL_OBJECT_FILES}" ]
  then
    echo "no source code found in ${KERNEL_DIR}"
    exit 1
  fi
  
  # echo "linking: ${KERNEL_ELF}"
  # COMMAND="${GCC} ${LDFLAGS} ${ALL_OBJECT_FILES} -o ${KERNEL_ELF}"
  # echo "${COMMAND}"
  # ${COMMAND}
  # 
  # echo "${AVR_SIZE} -C ${KERNEL_ELF}"
  # 
  # PROGRAM_SIZE=$(${AVR_SIZE} -C ${KERNEL_ELF} | grep "Program" |  tr -d -c 0-9)
  # PROGRAM_PERCENT=$((100*${PROGRAM_SIZE}/${MAX_PROGRAM_MEMORY}))
  # echo "*** Kernel size:"
  # echo "***  - Program memory: ${PROGRAM_SIZE} bytes (${PROGRAM_PERCENT}%, max: ${MAX_PROGRAM_MEMORY} bytes)"
  # DATA_SIZE=$(${AVR_SIZE} -C ${KERNEL_ELF} | grep "Data" |  tr -d -c 0-9)
  # RAM_PERCENT=$((100*${DATA_SIZE}/${MAX_RAM}))
  # echo "***  - Dynamic memory: ${DATA_SIZE} bytes (${RAM_PERCENT}%, max: ${MAX_RAM} bytes)"
  # echo "creating hex file: ${KERNEL_HEX_FILE}"
  # ${OBJ_COPY}  -j .text -j .data -O ihex ${KERNEL_ELF} ${KERNEL_HEX_FILE}
  # set +e
  
}




function build_app() {
  SOURCE_DIR=$APPS_DIR/$1
  echo "building application code in ${SOURCE_DIR}"
  mkdir -p ${BUILD_DIR}/apps/$1
  ALL_OBJECT_FILES=""
  APP_ELF="${BUILD_DIR}/apps/$1/$1.elf"
  APP_HEX_FILE="${BUILD_DIR}/apps/$1/$1.hex"
  set -e
  for SOURCE_FILE in ${SOURCE_DIR}/*.c
  do
    echo "compile: ${SOURCE_FILE}"
    BASENAME=$(basename -- "$SOURCE_FILE")
    FILENAME="${BASENAME%.*}"
    OBJ_FILE=${BUILD_DIR}/apps/$1/$FILENAME.o
    ALL_OBJECT_FILES="${ALL_OBJECT_FILES} ${OBJ_FILE}"
    COMMAND="${GCC} -c ${CFLAGS} ${INCLUDE_PATHS} -o ${OBJ_FILE} $SOURCE_FILE"
    #echo "${COMMAND}"
    ${COMMAND}
  done
  if [ -z "${ALL_OBJECT_FILES}" ]
  then
    echo "no source code found in ${SOURCE_DIR}"
    exit 1
  fi

  for OBJ_FILE in ${BUILD_DIR}/kernel/*.o
  do
    ALL_OBJECT_FILES="${ALL_OBJECT_FILES} ${OBJ_FILE}"
  done


  echo "linking: ${APP_ELF}"
  COMMAND="${GCC} ${LDFLAGS} ${ALL_OBJECT_FILES} -o ${APP_ELF}"
  echo "${COMMAND}"
  ${COMMAND}

  echo "${AVR_SIZE} -C ${APP_ELF}"

  PROGRAM_SIZE=$(${AVR_SIZE} -C ${APP_ELF} | grep "Program" |  tr -d -c 0-9)
  PROGRAM_PERCENT=$((100*${PROGRAM_SIZE}/${MAX_PROGRAM_MEMORY}))
  echo "*** App size:"
  echo "***  - Program memory: ${PROGRAM_SIZE} bytes (${PROGRAM_PERCENT}%, max: ${MAX_PROGRAM_MEMORY} bytes)"
  DATA_SIZE=$(${AVR_SIZE} -C ${APP_ELF} | grep "Data" |  tr -d -c 0-9)
  RAM_PERCENT=$((100*${DATA_SIZE}/${MAX_RAM}))
  echo "***  - Dynamic memory: ${DATA_SIZE} bytes (${RAM_PERCENT}%, max: ${MAX_RAM} bytes)"
  echo "creating hex file: ${APP_HEX_FILE}"
  ${OBJ_COPY}  -j .text -j .data -O ihex ${APP_ELF} ${APP_HEX_FILE}
  set +e
}



function upload_app() {
  APP_HEX_FILE="${BUILD_DIR}/apps/$1/$1.hex"
  
  set -e
  echo "ping chip"
  pymcuprog ping -t uart -d ${MCU} -u ${PROGRAMMER_DEVICE}


  echo "erase chip"
  pymcuprog erase -t uart -d ${MCU} -u ${PROGRAMMER_DEVICE}


  echo "write app image to chip"
  pymcuprog write -t uart -d ${MCU} -u ${PROGRAMMER_DEVICE} -f ${APP_HEX_FILE}

  set +e
}


rm -Rf $BUILD_DIR
mkdir -p $BUILD_DIR



build_kernel

build_app test2


echo "activate python virtualenv"
source hamster_pyenv/bin/activate

upload_app test2

echo "done"
exit





exit


set -e

echo "compile bootloader.c"
/Applications/microchip/xc8/v2.35/bin/xc8-cc -mcpu=attiny1626 -O2 -o ./hello.elf kernel/bootloader.c

echo "link to hex image"
/Applications/microchip/xc8/v2.35/bin/avr-objcopy -O ihex ./hello.elf ./hello.hex

echo "activate python virtualenv"
source karkom/bin/activate

echo "write image to chip"
pymcuprog write -t uart -d attiny1626 -u /dev/cu.usbserial-14310 -f bootloader/hello.hex


exit

example with XC8 compilation:

rm ./hello.elf
rm ./hello.hex
/Applications/microchip/xc8/v2.35/bin/xc8-cc -mcpu=attiny1626 -O2 -o ./hello.elf kernel/bootloader.c
/Applications/microchip/xc8/v2.35/bin/avr-objcopy -O ihex ./hello.elf ./hello.hex
pymcuprog write -t uart -d attiny1626 -u /dev/cu.usbserial-14110 -f ./hello.hex