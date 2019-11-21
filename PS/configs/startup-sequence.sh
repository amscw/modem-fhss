#!/bin/sh

if [ -d /sys/mfhss-dynamic ]; then
	echo 1 >> /sys/mfhss-dynamic/m/master # 1 - master / 0 - slave
	echo 2 >> /sys/mfhss-dynamic/m/mode # ("00" - Симплексный, "01" - Полудуплексный, "10" - Полнодуплексный)
	echo 100000 >> /sys/mfhss-dynamic/dlink/reqpack_tr_size
	echo 100 >> /sys/mfhss-dynamic/dlink/ackpack_tr_size
	echo 384 >> /sys/mfhss-dynamic/dlink/datapack_mdata_size
	echo 384 >> /sys/mfhss-dynamic/dlink/datapack_sdata_size
	echo 50 >> /sys/mfhss-dynamic/dlink/datapack_tr_size
	echo 0 >> /sys/mfhss-dynamic/dlink/coder_mod # BPSK_1_2
	echo 29 >> /sys/mfhss-dynamic/dlink/nmatch
	echo 0 >> /sys/mfhss-dynamic/dlink/en_lock_detect
	echo 0 >> /sys/mfhss-dynamic/watch/en # 0 - master / 1 - slave
	echo 1000 >> /sys/mfhss-dynamic/watch/wait_max
	echo 1000 >> /sys/mfhss-dynamic/watch/frz_max
	echo 0 >> /sys/mfhss-dynamic/phy/m_hop_en_link
	echo 0 >> /sys/mfhss-dynamic/phy/m_loop
	echo 1 >> /sys/mfhss-dynamic/phy/m_en_hop
	echo 4096 >> /sys/mfhss-dynamic/phy/m_hop_lookup_size
	echo 1 >> /sys/mfhss-dynamic/mlip/en_size
	echo 128 >> /sys/mfhss-dynamic/mlip/size_pack
	echo 16000 >> /sys/mfhss-dynamic/phy/m_afc_err_th
	echo 1 >> /sys/mfhss-dynamic/phy/m_afc_scale
	echo 6 >> /sys/mfhss-dynamic/phy/m_gard_th
	echo 6 >> /sys/mfhss-dynamic/phy/m_gard_mu_p
	echo 1 >> /sys/mfhss-dynamic/phy/m_nco_frz # 1 - master / 0 - slave
	echo 0 >> /sys/mfhss-dynamic/phy/ic_att_mode
	echo 0 >> /sys/mfhss-dynamic/phy/ic_att_value
	echo 200 >> /sys/mfhss-dynamic/phy/m_hop_est_max
	echo 1 >> /sys/mfhss-dynamic/sap/loop	
	# Ключи
	echo 1 >> /sys/mfhss-dynamic/phy/m_hop_seed
	echo 1 >> /sys/mfhss-dynamic/dlink/coder_seed
	echo 1 >> /sys/mfhss-dynamic/sap/key_sap
	echo 19088743 >> /sys/mfhss-dynamic/sap/key_intr
	echo 5 >> /sys/mfhss-dynamic/sap/size_pr
	echo 100 >> /sys/mfhss-dynamic/sap/size_pack
	echo 1 >> /sys/mfhss-dynamic/dlink/ber_en
	echo 1 >> /sys/mfhss-dynamic/sap/en
	echo 1 >> /sys/mfhss-dynamic/dlink/coder_en
	echo 1 >> /sys/mfhss-dynamic/m/rst
fi
