#!/bin/sh

if [ -d /sys/mfhss-dynamic ]; then
	echo 1 >> /sys/mfhss-dynamic/m/master # 1 - master / 0 - slave
	echo 1 >> /sys/mfhss-dynamic/m/mode
	echo 100000 >> /sys/mfhss-dynamic/dlink/reqpack_tr_size
	echo 100 >> /sys/mfhss-dynamic/dlink/ackpack_tr_size
	echo 400 >> /sys/mfhss-dynamic/dlink/datapack_mdata_size
	echo 400 >> /sys/mfhss-dynamic/dlink/datapack_sdata_size
	echo 50 >> /sys/mfhss-dynamic/dlink/datapack_tr_size
	echo 29 >> /sys/mfhss-dynamic/dlink/nmatch
	echo 0 >> /sys/mfhss-dynamic/dlink/en_lock_detect
	echo 0 >> /sys/mfhss-dynamic/watch/en # 0 - master / 1 - slave
	echo 1000 >> /sys/mfhss-dynamic/watch/wait_max
	echo 1000 >> /sys/mfhss-dynamic/watch/frz_max
	echo 0 >> /sys/mfhss-dynamic/phy/m_hop_en_link
	echo 0 >> /sys/mfhss-dynamic/phy/m_loop
	echo 1 >> /sys/mfhss-dynamic/phy/m_en_hop
	echo 1024 >> /sys/mfhss-dynamic/phy/m_hop_lookup_size
	echo 0 >> /sys/mfhss-dynamic/mlip/en_size
	echo 128 >> /sys/mfhss-dynamic/mlip/size_pack
	echo 16000 >> /sys/mfhss-dynamic/phy/m_afc_err_th
	echo 1 >> /sys/mfhss-dynamic/phy/m_afc_scale
	echo 6 >> /sys/mfhss-dynamic/phy/m_gard_th
	echo 6 >> /sys/mfhss-dynamic/phy/m_gard_mu_p
	echo 1 >> /sys/mfhss-dynamic/phy/m_nco_frz # 1 - master / 0 - slave
	echo 0 >> /sys/mfhss-dynamic/phy/ic_att_mode
	echo 0 >> /sys/mfhss-dynamic/phy/ic_att_value
	echo 1 >> /sys/mfhss-dynamic/m/rst
fi
