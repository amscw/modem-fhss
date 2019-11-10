#
# SecBus ( http://secbus.telecom-paristech.fr/ ) - This file is part of SecBus
# Copyright (C) - Telecom ParisTech
# Contacts: contact-secbus@telecom-paristech.fr
#
# This file must be used under the terms of the CeCILL.
# This source file is licensed as described in the file COPYING, which
# you should have received as part of this distribution. The terms
# are also available at
# http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.txt
#

if { $argc != 2 } {
	puts "usage: fsbl.hsi.tcl <hardware-description-file> <fsbl-build-directory>"
} else {
	set hdf [lindex $argv 0]
	set dir [lindex $argv 1]
	set design [ open_hw_design ${hdf} ]
	generate_app -hw $design -os standalone -proc ps7_cortexa9_0 -app zynq_fsbl -compile -sw fsbl -dir ${dir}
}
