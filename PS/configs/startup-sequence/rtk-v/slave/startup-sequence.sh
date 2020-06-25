#!/bin/sh

if [ -d /sys/mfhss-dynamic ]; then
	echo 1 >> /sys/mfhss-dynamic/m/rst
	#echo 0 >> /sys/mfhss-dynamic/m/master # 1 - master / 0 - slave
	echo 1 >> /sys/mfhss-dynamic/m/en # разрешение работы
	echo 0 >> /sys/mfhss-dynamic/dlink/loopback # заворотка в DLINK
	echo 0 >> /sys/mfhss-dynamic/dlink/stream # потоковый режим (отладочный)
	echo 20 >> /sys/mfhss-dynamic/dlink/size_m # кол-во символов в фрейме мастера
 	echo 20 >> /sys/mfhss-dynamic/dlink/size_s # кол-во символов в фрейме раба
	echo 7 >> /sys/mfhss-dynamic/dlink/reserve
	# 0 - BPSK 1/2; 1 - QPSK 1/2; 2 - QPSK 3/4; 3 - QAM16 1/2; 4 - QAM16 3/4; 5 - QAM64 2/3; 6 - QAM64 3/4
	echo 1 >> /sys/mfhss-dynamic/dlink/code_mod_m # код модуляции мастера
	echo 1 >> /sys/mfhss-dynamic/dlink/code_mod_s # код модуляции раба
	echo 0 >> /sys/mfhss-dynamic/phy/loopback
	echo 1 >> /sys/mfhss-dynamic/phy/en_tx
	echo 1 >> /sys/mfhss-dynamic/phy/en_rx
	# KEY
	# CI
	echo 1 >> /sys/mfhss-dynamic/ci/en
	echo 9758402 >> /sys/mfhss-dynamic/ci/syncro
	#echo 666 >> /sys/mfhss-dynamic/ci/key_lsb
	#echo 777 >> /sys/mfhss-dynamic/ci/key_msb
	echo 320 >> /sys/mfhss-dynamic/ci/size_pack
	echo 5 >> /sys/mfhss-dynamic/ci/size_pr
	# SAP
	echo 1 >> /sys/mfhss-dynamic/sap/en
	#echo 100 >> /sys/mfhss-dynamic/sap/key_sap # 23130 0x5A5A
	#echo 1985229328 >> /sys/mfhss-dynamic/sap/key_intr # 1985229328 0x76543210
	echo 400 >> /sys/mfhss-dynamic/sap/size_pack
	# DLINK
	echo 1 >> /sys/mfhss-dynamic/dlink/key_en
	echo 100 >> /sys/mfhss-dynamic/dlink/key_m # 100
	echo 200 >> /sys/mfhss-dynamic/dlink/key_s # 200
	echo 1 >> /sys/mfhss-dynamic/dlink/key_en_common
	#echo 55 >> /sys/mfhss-dynamic/dlink/key_common # 55 ключ скремблера в обоих направлениях
	# PHY
	echo 1 >> /sys/mfhss-dynamic/phy/key_en
	echo 1365 >> /sys/mfhss-dynamic/phy/key_m # 1365 ключ скремблера для мастера
	echo 682 >> /sys/mfhss-dynamic/phy/key_s # 682 ключ скремблера для раба
	echo 1 >> /sys/mfhss-dynamic/phy/key_en_common
	#echo 555 >> /sys/mfhss-dynamic/phy/key_common # 555 ключ скремблера в обоих направлениях
	# SENS
	echo 1 >> /sys/mfhss-dynamic/sens/en
	echo 10000000 >> /sys/mfhss-dynamic/sens/size
	# IC
	echo 1 >> /sys/mfhss-dynamic/ic/att_mode
	echo 0 >> /sys/mfhss-dynamic/ic/att_att_msb
	echo 0 >> /sys/mfhss-dynamic/ic/att_att_lsb
	echo 60 >> /sys/mfhss-dynamic/ic/att_th_upper # порог, когда большое усиление (слабый сигнал) и нужно усилить через att
	echo 20 >> /sys/mfhss-dynamic/ic/att_th_lower # порог, когда малое усиление (большой сигнал) и нужно ослабить через att
	echo 32 >> /sys/mfhss-dynamicic/att_cnt_thr # порог счетчика перегрузок и недогрузок, когда нужно менять усиление через att
	echo 0 >> /sys/mfhss-dynamic/ic/manual_en
	echo 0 >> /sys/mfhss-dynamic/ic/manual_pow_en
	echo 0 >> /sys/mfhss-dynamic/ic/tx_pin_on
	echo 0 >> /sys/mfhss-dynamic/ic/rx_pin_on
	echo 0 >> /sys/mfhss-dynamic/ic/pow_en
	echo 4096 >> /sys/mfhss-dynamic/ic/reserve_power
	echo 1000 >> /sys/mfhss-dynamic/ic/reserve_ad
	echo 0 >> /sys/mfhss-dynamic/ic/ad_manual_en
	# MGC
	echo 1 >> /sys/mfhss-dynamic/mgc/mode
	echo 10 >> /sys/mfhss-dynamic/mgc/step_large_inc
	echo 10 >> /sys/mfhss-dynamic/mgc/step_large_dec
	echo 2 >> /sys/mfhss-dynamic/mgc/step_small_inc
	echo 2 >> /sys/mfhss-dynamic/mgc/step_small_dec
	echo 96 >> /sys/mfhss-dynamic/mgc/upper_large_thr # 6
	echo 0 >> /sys/mfhss-dynamic/mgc/upper_small_thr # 0
	echo 3936 >> /sys/mfhss-dynamic/mgc/lower_small_thr # -10
	echo 3808 >> /sys/mfhss-dynamic/mgc/lower_large_thr # -18
	echo 6 >> /sys/mfhss-dynamic/mgc/upper_large_cnt
	echo 6 >> /sys/mfhss-dynamic/mgc/upper_small_cnt
	echo 6 >> /sys/mfhss-dynamic/mgc/lower_small_cnt
	echo 6 >> /sys/mfhss-dynamic/mgc/lower_large_cnt
	echo 16 >> /sys/mfhss-dynamic/mgc/update_cnt
	echo 32 >> /sys/mfhss-dynamic/mgc/timeout
	echo 160 >> /sys/mfhss-dynamic/mgc/locked_cnt
	echo 0 >> /sys/mfhss-dynamic/mgc/pulse_two_mode
	echo 0 >> /sys/mfhss-dynamic/mgc/synch_use
	echo 1 >> /sys/mfhss-dynamic/mgc/fb_gain_mode
	# BER
	echo 0 >> /sys/mfhss-dynamic/ber/en
	echo 0 >> /sys/mfhss-dynamic/ber/const
	echo 100000000 >> /sys/mfhss-dynamic/ber/size
	echo 200000000 >> /sys/mfhss-dynamic/ber/size_ch
	# RST
	echo 0 >> /sys/mfhss-dynamic/m/rst
fi
