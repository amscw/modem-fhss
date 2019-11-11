#!/bin/bash
PROJECT_DIR=/home/alex-m/projects/
DT_XLNX_DIR=/home/alex-m/build/device-tree-xlnx/

if ! [[ $1 ]]; then 
	echo "Usage: $0 dts|dtb"
	exit 1
fi

if ! [[ -d $DT_XLNX_DIR ]]; then
	echo "no such directory: $DT_XLNX_DIR"
	exit 2
fi

cd $(dirname $0)/../..
FW_DIR=$(pwd)
echo "firmware directory: $FW_DIR"

cd PS/hw/dt 

case $1 in
	"dts")
		echo "import .sysdef"
		cp  $FW_DIR/PL/output/system_top.sysdef ./system_top.sysdef
		echo "make DTS..."
		hsi -mode batch -quiet -notrace -source ../scripts/dts.hsi.tcl -tclargs ./system_top.sysdef $DT_XLNX_DIR .
		# cleanup 
		rm -rf .Xil
		rm -rf drivers
		rm ./system_top.sysdef
		rm ./system_top.bit
		find . -name '*.jou' -type f -delete
		find . -name '*.log' -type f -delete
		find . -name 'ps7_init*' -type f -delete
		;;
	"dtb")
		echo "make DTB..."
		dtc -I dts -O dtb -o devicetree.dtb ./system.dts
		;;
	*)
		echo "bad parameter: $1"
		exit 3
		;;
esac

exit 0
