#!/bin/bash
# WARNING! need: u-boot in $UBOOT_DIR

UBOOT_DIR=/home/alex-m/build/u-boot-xlnx

if ! [[ -d $UBOOT_DIR ]]; then
	echo "no such directory: $UBOOT_DIR"
	exit 1
fi

cd $(dirname $0)/../..
FW_DIR=$(pwd)
echo "firmware directory: $FW_DIR"

# import bit
echo "import .bit"
cp ./PL/output/system_top.bit ./PS/hw/boot/bitstream.bit

# make fsbl
echo "make fsbl..."
cd PS/hw/fsbl
rm -rf ./*
cp $FW_DIR/PL/output/system_top.sysdef ./system_top.sysdef
hsi -mode batch -quiet -notrace -source ../scripts/fsbl.hsi.tcl -tclargs system_top.sysdef .
if ! [[ -f ./executable.elf ]]; then
	echo "cannot create fsbl"
	exit 3
else 
	cp ./executable.elf ../boot/fsbl.elf
fi

#cleanup
rm -rf .Xil
find . -name '*.o' -type f -delete
find . -name '*.jou' -type f -delete
find . -name '*.log' -type f -delete
rm system_top.bit
rm system_top.sysdef
		
cd ../boot

# import u-boot
echo "import u-boot"
if ! [[ -f $UBOOT_DIR/u-boot ]]; then
	echo "need u-boot in $UBOOT_DIR"
	exit 4
else 
	cp $UBOOT_DIR/u-boot ./u-boot.elf
fi

# make BOOT.bin
echo "make BOOT.bin"
bootgen -w -image ../scripts/boot.bif -o BOOT.bin

exit 0
