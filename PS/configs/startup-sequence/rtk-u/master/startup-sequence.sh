#!/bin/sh

if [ -d /sys/mfhss-dynamic ]; then
	echo 1 >> /sys/mfhss-dynamic/m/rst
	echo 1 >> /sys/mfhss-dynamic/afe/rst
	echo 1 >> /sys/mfhss-dynamic/m/mode # ("00" - Симплексный, "01" - Полудуплексный, "10" - Полнодуплексный)
	echo 55 >> /sys/mfhss-dynamic/mlip/dflt_value 
	# Настройка DLINK
	echo 307200 >> /sys/mfhss-dynamic/dlink/reqpack_tr_size
	echo 64 >> 	/sys/mfhss-dynamic/dlink/ackpack_tr_size
	echo 7680 >> /sys/mfhss-dynamic/dlink/datapack_mdata_size
	echo 7680 >> /sys/mfhss-dynamic/dlink/datapack_sdata_size
	echo 100 >> /sys/mfhss-dynamic/dlink/datapack_tr_size
	echo 100 >> /sys/mfhss-dynamic/dlink/est_start
	echo 200 >> /sys/mfhss-dynamic/dlink/est_max
	echo 50 >> 	/sys/mfhss-dynamic/dlink/nmatch
	# Настройки BER
	echo 0 >> 			/sys/mfhss-dynamic/ber/en
	echo 0 >> 			/sys/mfhss-dynamic/ber/const
	echo 10000 >> 	/sys/mfhss-dynamic/ber/est_len
	echo 360 >> 		/sys/mfhss-dynamic/ber/size_pack
	echo 0 >> 			/sys/mfhss-dynamic/ber/timeout_en
	echo 100000 >> 		/sys/mfhss-dynamic/ber/timeout
	echo 20000 >> 	/sys/mfhss-dynamic/ber/coder_est_len
	echo 768 >> 		/sys/mfhss-dynamic/ber/coder_size_pack
	echo 0 >> 			/sys/mfhss-dynamic/ber/coder_timeout_en
	# Настройки DIGAGC
	echo 869 >> 	/sys/mfhss-dynamic/digagc/mult
	echo 16 >> 		/sys/mfhss-dynamic/digagc/mma_deep_st
	# Настройки SYMSYNCH
	echo 1500 >> /sys/mfhss-dynamic/sym/bm_th
	echo 19 >> 	/sys/mfhss-dynamic/sym/tune_cnt
	# Настройки задержек PHY
	# Задержки в символах
	echo 700 >> /sys/mfhss-dynamic/phy/rsv_for_rxtx
	echo 10 >> 	/sys/mfhss-dynamic/phy/rsv_for_fs
	# Задержки в семплах
	echo 580 >> /sys/mfhss-dynamic/phy/delay_mod_hop
	echo 400 >> /sys/mfhss-dynamic/phy/delay_demod_hop
	echo 200 >> 	/sys/mfhss-dynamic/phy/rsv_for_pow
	echo 100 >> /sys/mfhss-dynamic/phy/rsv_for_demod
	echo 180 >> /sys/mfhss-dynamic/phy/rsv_for_expander
	# Настройки PWRKSV
	echo 0 >> 	/sys/mfhss-dynamic/pwr/rl_en
	echo 1016 >> /sys/mfhss-dynamic/pwr/rl_th
	echo 0 >> 	/sys/mfhss-dynamic/pwr/vgg_en
	echo 600 >> /sys/mfhss-dynamic/pwr/vgg_low
	echo 850 >> /sys/mfhss-dynamic/pwr/vgg_high
	echo 700 >> /sys/mfhss-dynamic/pwr/vgg_dflt
	echo 10 >> 	/sys/mfhss-dynamic/pwr/vgg_incr
	echo 100000 >> /sys/mfhss-dynamic/pwr/vgg_timeout
	echo 2900 >> 	/sys/mfhss-dynamic/pwr/cpldf_low
	echo 3950 >> 	/sys/mfhss-dynamic/pwr/cpldf_high
	#Настройки ATT
	echo 1 >> 		/sys/mfhss-dynamic/att/mode
	echo 9440 >> 	/sys/mfhss-dynamic/att/upd_cnt
	echo 10 >> 		/sys/mfhss-dynamic/att/mma_deep_st
	echo 25000 >> 	/sys/mfhss-dynamic/att/upd_cnt_link
	echo 14 >> 		/sys/mfhss-dynamic/att/mma_deep_st_link
	
	echo 1152 >> 	/sys/mfhss-dynamic/att/lvl_0
	echo 384 >> 	/sys/mfhss-dynamic/att/lvl_1
	echo -512 >> 	/sys/mfhss-dynamic/att/lvl_2
	echo -3000 >> 	/sys/mfhss-dynamic/att/lvl_3

	echo 1152 >> 	/sys/mfhss-dynamic/att/lvl_link_0
	echo 384 >> 	/sys/mfhss-dynamic/att/lvl_link_1
	echo -512 >> 	/sys/mfhss-dynamic/att/lvl_link_2
	echo -3000 >> 	/sys/mfhss-dynamic/att/lvl_link_3

	echo 18 >> 		/sys/mfhss-dynamic/att/step_0
	echo 4 >> 		/sys/mfhss-dynamic/att/step_1
	echo 2 >> 		/sys/mfhss-dynamic/att/step_2
	echo 4 >> 		/sys/mfhss-dynamic/att/step_3
	echo 9 >> 		/sys/mfhss-dynamic/att/step_4

	echo 18 >> 		/sys/mfhss-dynamic/att/step_link_0
	echo 4 >> 		/sys/mfhss-dynamic/att/step_link_1
	echo 2 >> 		/sys/mfhss-dynamic/att/step_link_2
	echo 4 >> 		/sys/mfhss-dynamic/att/step_link_3
	echo 9 >> 		/sys/mfhss-dynamic/att/step_link_4

	echo -256 >> 		/sys/mfhss-dynamic/att/hyst_lower_in
	echo 128 >> 		/sys/mfhss-dynamic/att/hyst_upper_in
	echo -384 >> 		/sys/mfhss-dynamic/att/hyst_lower_out
	echo 384 >> 		/sys/mfhss-dynamic/att/hyst_upper_out

	echo -256 >> 		/sys/mfhss-dynamic/att/hyst_lower_in_link
	echo 128 >> 		/sys/mfhss-dynamic/att/hyst_upper_in_link
	echo -384 >> 		/sys/mfhss-dynamic/att/hyst_lower_out_link
	echo 384 >> 		/sys/mfhss-dynamic/att/hyst_upper_out_link

	echo 15 >> 		/sys/mfhss-dynamic/att/value_0
	echo 0 >> 		/sys/mfhss-dynamic/att/value_1
	echo 0 >> 		/sys/mfhss-dynamic/att/value_2
	echo 0 >> 		/sys/mfhss-dynamic/att/fix_max
	echo 0 >> 		/sys/mfhss-dynamic/att/fix_step_up
	echo 0 >> 		/sys/mfhss-dynamic/att/fix_step_down
	echo 0 >> 		/sys/mfhss-dynamic/att/fix_start
	# Настройка CI
	echo 10 >> 	/sys/mfhss-dynamic/ci/size_pr
	echo 160 >> /sys/mfhss-dynamic/ci/size_pack
	echo 1 >> 	/sys/mfhss-dynamic/ci/en
	echo 0 >> 	/sys/mfhss-dynamic/ci/loop
	echo 123 >> 	/sys/mfhss-dynamic/ci/syncro
	# Настройка SAP
	echo 5 >> 	/sys/mfhss-dynamic/sap/size_pr
	echo 100 >> /sys/mfhss-dynamic/sap/size_pack
	echo 1 >> 	/sys/mfhss-dynamic/sap/en
	# Настройка Coder
	echo 0 >> /sys/mfhss-dynamic/dlink/coder_mod # BPSK_1_2
	echo 1 >> /sys/mfhss-dynamic/dlink/coder_en
	# Настройка AFE	
	echo 1 >> 	/sys/mfhss-dynamic/afe/nrst
	echo 0 >> 	/sys/mfhss-dynamic/afe/ch_locked
	echo 20000 >> /sys/mfhss-dynamic/afe/wait_cnt
	sleep 1
	echo 155 >> /sys/mfhss-dynamic/afe/clk_psdata
	echo 0 >> 	/sys/mfhss-dynamic/afe/idelay_data
	echo 0 >> 	/sys/mfhss-dynamic/afe/rst
	# Настройка DC removal	
	echo 0 >> 		/sys/mfhss-dynamic/dc/man
	echo 23 >> 		/sys/mfhss-dynamic/dc/mma_deep
	echo -100 >> 	/sys/mfhss-dynamic/dc/re
	echo -100 >> 	/sys/mfhss-dynamic/dc/im
	# HOPPER
	echo 1 >> 			/sys/mfhss-dynamic/hop/en_large
	echo 1 >> 			/sys/mfhss-dynamic/hop/en_fhss
	echo 687194767 >> 	/sys/mfhss-dynamic/hop/freq_manual
	echo 0 >> 			/sys/mfhss-dynamic/hop/en_fd
	echo 0 >> 			/sys/mfhss-dynamic/hop/en_slip
	echo 300 >> 		/sys/mfhss-dynamic/hop/timeout_aft_slip
	echo 1024 >> 		/sys/mfhss-dynamic/hop/synch_size
	echo 0 >> 			/sys/mfhss-dynamic/hop/fd_num_freq_in_filt
	# Дополнительно
	echo 0 >> 	/sys/mfhss-dynamic/m/dds_en
	echo 50 >> 	/sys/mfhss-dynamic/m/dds_phase
	echo 1 >> 	/sys/mfhss-dynamic/m/ce_mux
	echo 0 >> 	/sys/mfhss-dynamic/m/phy_loop

	#
	echo 0 >> /sys/mfhss-dynamic/m/rst
fi
