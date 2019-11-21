#!/bin/bash
#WARNING start this script from PS/ working directory
#WARNING! need: qspi-flash, srec_cat 
#WARNING:
# boot.bin must present in hw/boot/
# devicetree.dtb must present in hw/dt/
# linux kernel and rootfs were built

BUILDROOT_DIR=/home/alex-m/build/buildroot
SDK_DIR=/home/alex-m/vivado/SDK/2016.2/bin

if ! [[ -d $BUILDROOT_DIR ]]; then
	echo "no such directory: $BUILDROOT_DIR"
	exit 1
fi

cd $(dirname $0)/../..
FW_DIR=$(pwd)
echo "firmware directory: $FW_DIR"

cd PS

if ! [[ -d image ]] 
then
	mkdir image
else
	rm -rf image/*
fi

cp ./hw/boot/BOOT.bin image/
cp ./hw/dt/devicetree.dtb image/
cp $BUILDROOT_DIR/output/images/uImage ./image/
cp $BUILDROOT_DIR/output/images/rootfs.cpio.uboot ./image/uramdisk.image.gz

cd image

function mkimg()
{
	#соединяет все файлы образа в один файл образа, смещая их по указанным адресам
	qspi-flash 	BOOT.bin 0x000000\
				uImage 0x700000\
				devicetree.dtb 0xB00000\
				uramdisk.image.gz 0xB20000\
				-o image.bin 0x2000000	
}

if [[ $1 ]]; then
	case $1 in
		"bin") 
			mkimg
			;;
		"mcs") 
			mkimg
			srec_cat image.bin -Binary -o image.mcs -Intel
			rm -f image.bin
			;;
		"flash")
			if ! [[ -d $SDK_DIR ]]; then
				echo "no such directory: $SDK_DIR"
				exit 2
			fi
			mkimg
			# программирует бинарник в таргет
			$SDK_DIR/program_flash -f $FW_DIR/PS/image/image.bin\
				-flash_type qspi_single -verify -cable type xilinx_tcf			
			rm -f $FW_DIR/PS/image/image.bin
			;;
		*) 
			echo "bad parameter: $1"
			;;
	esac
fi

exit 0
