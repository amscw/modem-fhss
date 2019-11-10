import os
import sys

def modemSendData(data):
	dev = os.open("/dev/modem-fhss", os.O_RDWR)
	os.write(dev, data.encode())
	read_data = os.read(dev, sys.getsizeof(data))
	os.close(dev)
	return read_data.decode()

def setModemRst(val):
	with open('/sys/modem-fhss/modem_rst', 'w') as f:
		f.write(str(val))

def setModemLoop(val):
	with open('/sys/modem-fhss/modem_loop', 'w') as f:
		f.write(str(val))

#def setModemLookDet(val):
#	with open('/sys/modem-fhss/modem_look_det', 'w') as f:
#		f.write(str(val))

def setModemEnMod(val):
	with open('/sys/modem-fhss/modem_en_mod', 'w') as f:
		f.write(str(val))

def setModemEnDemod(val):
	with open('/sys/modem-fhss/modem_en_demod', 'w') as f:
		f.write(str(val))

def setModemEnHopper(val):
	with open('/sys/modem-fhss/modem_en_hopper', 'w') as f:
		f.write(str(val))

def setModemSelFreq(val):
	with open('/sys/modem-fhss/modem_sel_freq', 'w') as f:
		f.write(str(val))

def setModemSeed(val):
	with open('/sys/modem-fhss/modem_seed', 'w') as f:
		f.write(str(val))

def setModemAvSize(val):
	with open('/sys/modem-fhss/modem_av_size', 'w') as f:
		f.write(str(val))

def setModemDecPrd(val):
	with open('/sys/modem-fhss/modem_dec_prd', 'w') as f:
		f.write(str(val))

def setModemThFreq(val):
	with open('/sys/modem-fhss/modem_th_freq', 'w') as f:
		f.write(str(val))

def setModemSawGen(val):
	with open('/sys/modem-fhss/modem_saw_gen', 'w') as f:
		f.write(str(val))

def setModemLookupSize(val):
	with open('/sys/modem-fhss/modem_lookup_size', 'w') as f:
		f.write(str(val))

def setModemAdEn(val):
	with open('/sys/modem-fhss/modem_ad_en', 'w') as f:
		f.write(str(val))

def setModemAdTxnrx(val):
	with open('/sys/modem-fhss/modem_ad_txnrx', 'w') as f:
		f.write(str(val))

def setModemAdInc(val):
	with open('/sys/modem-fhss/modem_ad_inc', 'w') as f:
		f.write(str(val))

def setModemAdDec(val):
	with open('/sys/modem-fhss/modem_ad_dec', 'w') as f:
		f.write(str(val))

def setModemAdBustConf(val):
	with open('/sys/modem-fhss/modem_ad_bust_conf', 'w') as f:
		f.write(str(val))

#def setModemAdLight(val):
#	with open('/sys/modem-fhss/modem_ad_light', 'w') as f:
#		f.write(str(val))

def setModemCicEn(val):
	with open('/sys/modem-fhss/modem_cic_en', 'w') as f:
		f.write(str(val))

def setModemDcDelTx(val):
	with open('/sys/modem-fhss/modem_dc_del_tx', 'w') as f:
		f.write(str(val))

def setModemDcDelRx(val):
	with open('/sys/modem-fhss/modem_dc_del_rx', 'w') as f:
		f.write(str(val))

def setModemAfcEn(val):
	with open('/sys/modem-fhss/modem_afc_en', 'w') as f:
		f.write(str(val))

def setModemAfcKp(val):
	with open('/sys/modem-fhss/modem_afc_kp', 'w') as f:
		f.write(str(val))

def setModemAfcKi(val):
	with open('/sys/modem-fhss/modem_afc_ki', 'w') as f:
		f.write(str(val))

def setModemGardMuP(val):
	with open('/sys/modem-fhss/modem_gard_mu_p', 'w') as f:
		f.write(str(val))

def setModemGardStart(val):
	with open('/sys/modem-fhss/modem_gard_start', 'w') as f:
		f.write(str(val))

def setModemGardLenFrz(val):
	with open('/sys/modem-fhss/modem_gard_len_frz', 'w') as f:
		f.write(str(val))

#added by moskvin 01.09.2017
def setModemHopperThcnt(val):
	with open('/sys/modem-fhss/modem_hopper_thcnt', 'w') as f:
		f.write(str(val))

def setModemHopperSlip(val):
	with open('/sys/modem-fhss/modem_hopper_slip', 'w') as f:
		f.write(str(val))

def setModemMlipLoop(val):
	with open('/sys/modem-fhss/modem_mlip_loop', 'w') as f:
		f.write(str(val))

def setModemMlipCntcrcOn(val):
	with open('/sys/modem-fhss/modem_mlip_cntcrc_on', 'w') as f:
		f.write(str(val))

# def setModemMlipCntcrc(val):
# 	with open('/sys/modem-fhss/modem_mlip_cntcrc', 'w') as f:
# 		f.write(str(val))

# def setModemMlipCntTx(val):
# 	with open('/sys/modem-fhss/modem_mlip_cnt_tx', 'w') as f:
# 		f.write(str(val))

# def setModemMlipCntRx(val):
# 	with open('/sys/modem-fhss/modem_mlip_cnt_rx', 'w') as f:
# 		f.write(str(val))

def setModemGardSlipFw(val):
	with open('/sys/modem-fhss/modem_gard_slip_fw', 'w') as f:
		f.write(str(val))

def setModemGardSlipBw(val):
	with open('/sys/modem-fhss/modem_gard_slip_bw', 'w') as f:
		f.write(str(val))		
#end added		

def getModemRst():
	with open('/sys/modem-fhss/modem_rst') as f:
		return int(f.read())

def getModemLoop():
	with open('/sys/modem-fhss/modem_loop') as f:
		return int(f.read())

def getModemLookDet():
	with open('/sys/modem-fhss/modem_look_det') as f:
		return int(f.read())

def getModemEnMod():
	with open('/sys/modem-fhss/modem_en_mod') as f:
		return int(f.read())

def getModemEnDemod():
	with open('/sys/modem-fhss/modem_en_demod') as f:
		return int(f.read())

def getModemEnHopper():
	with open('/sys/modem-fhss/modem_en_hopper') as f:
		return int(f.read())

def getModemSelFreq():
	with open('/sys/modem-fhss/modem_sel_freq') as f:
		return int(f.read())

def getModemSeed():
	with open('/sys/modem-fhss/modem_seed') as f:
		return int(f.read())

def getModemAvSize():
	with open('/sys/modem-fhss/modem_av_size') as f:
		return int(f.read())

def getModemDecPrd():
	with open('/sys/modem-fhss/modem_dec_prd') as f:
		return int(f.read())

def getModemThFreq():
	with open('/sys/modem-fhss/modem_th_freq') as f:
		return int(f.read())

def getModemSawGen():
	with open('/sys/modem-fhss/modem_saw_gen') as f:
		return int(f.read())

def getModemLookupSize():
	with open('/sys/modem-fhss/modem_lookup_size') as f:
		return int(f.read())

def getModemAdEn():
	with open('/sys/modem-fhss/modem_ad_en') as f:
		return int(f.read())

def getModemAdTxnrx():
	with open('/sys/modem-fhss/modem_ad_txnrx') as f:
		return int(f.read())

def getModemAdInc():
	with open('/sys/modem-fhss/modem_ad_inc') as f:
		return int(f.read())

def getModemAdDec():
	with open('/sys/modem-fhss/modem_ad_dec') as f:
		return int(f.read())

def getModemAdBustConf():
	with open('/sys/modem-fhss/modem_ad_bust_conf') as f:
		return int(f.read())

def getModemAdLight():
	with open('/sys/modem-fhss/modem_ad_light') as f:
		return int(f.read())

def getModemCicEn():
	with open('/sys/modem-fhss/modem_cic_en') as f:
		return int(f.read())

def getModemDcDelTx():
	with open('/sys/modem-fhss/modem_dc_del_tx') as f:
		return int(f.read())

def getModemDcDelRx():
	with open('/sys/modem-fhss/modem_dc_del_rx') as f:
		return int(f.read())

def getModemAfcEn():
	with open('/sys/modem-fhss/modem_afc_en') as f:
		return int(f.read())

def getModemAfcKp():
	with open('/sys/modem-fhss/modem_afc_kp') as f:
		return int(f.read())

def getModemAfcKi():
	with open('/sys/modem-fhss/modem_afc_ki') as f:
		return int(f.read())

def getModemGardMuP():
	with open('/sys/modem-fhss/modem_gard_mu_p') as f:
		return int(f.read())

def getModemGardStart():
	with open('/sys/modem-fhss/modem_gard_start') as f:
		return int(f.read())

def getModemGardLenFrz():
	with open('/sys/modem-fhss/modem_gard_len_frz') as f:
		return int(f.read())

#added by moskvin 01.09.2017
def getModemHopperThcnt():
	with open('/sys/modem-fhss/modem_hopper_thcnt') as f:
		return int(f.read())

def getModemHopperSlip():
	with open('/sys/modem-fhss/modem_hopper_slip') as f:
		return int(f.read())

def getModemMlipLoop():
	with open('/sys/modem-fhss/modem_mlip_loop') as f:
		return int(f.read())

#added by moskvin 11.09.2017
def getModemMlipCntcrcOn():
	with open('/sys/modem-fhss/modem_mlip_cntcrc_on') as f:
		return int(f.read())

def getModemMlipCntcrc():
	with open('/sys/modem-fhss/modem_mlip_cntcrc') as f:
		return int(f.read())

def getModemMlipCntTx():
	with open('/sys/modem-fhss/modem_mlip_cnt_tx') as f:
		return int(f.read())

def getModemMlipCntRx():
	with open('/sys/modem-fhss/modem_mlip_cnt_rx') as f:
		return int(f.read())

def getModemGardSlipFw():
	with open('/sys/modem-fhss/modem_gard_slip_fw') as f:
		return int(f.read())

def getModemGardSlipBw():
	with open('/sys/modem-fhss/modem_gard_slip_bw') as f:
		return int(f.read())
