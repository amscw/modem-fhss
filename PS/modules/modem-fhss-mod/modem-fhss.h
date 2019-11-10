#ifndef _MODEM_OFDM_H_
#define _MODEM_OFDM_H_

#include <linux/fs.h>

static struct modem_fhss_attr {
	struct attribute attr;
	int value;

	struct chrdrv_private *priv;
};

static struct modem_fhss_attr modem_rst = {
	.attr.name = "modem_rst",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_loop = {
	.attr.name = "modem_loop",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_look_det = {
	.attr.name = "modem_look_det",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_en_mod = {
	.attr.name = "modem_en_mod",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_en_demod = {
	.attr.name = "modem_en_demod",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_en_hopper = {
	.attr.name = "modem_en_hopper",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_sel_freq = {
	.attr.name = "modem_sel_freq",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_seed = {
	.attr.name = "modem_seed",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_av_size = {
	.attr.name = "modem_av_size",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_dec_prd = {
	.attr.name = "modem_dec_prd",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_th_freq = {
	.attr.name = "modem_th_freq",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_saw_gen = {
	.attr.name = "modem_saw_gen",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_lookup_size = {
	.attr.name = "modem_lookup_size",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_ad_en = {
	.attr.name = "modem_ad_en",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_ad_txnrx = {
	.attr.name = "modem_ad_txnrx",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_ad_inc = {
	.attr.name = "modem_ad_inc",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_ad_dec = {
	.attr.name = "modem_ad_dec",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_ad_bust_conf = {
	.attr.name = "modem_ad_bust_conf",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_ad_light = {
	.attr.name = "modem_ad_light",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_cic_en = {
	.attr.name = "modem_cic_en",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_dc_del_tx = {
	.attr.name = "modem_dc_del_tx",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_dc_del_rx = {
	.attr.name = "modem_dc_del_rx",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_afc_en = {
	.attr.name = "modem_afc_en",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_afc_kp = {
	.attr.name = "modem_afc_kp",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_afc_ki = {
	.attr.name = "modem_afc_ki",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_gard_mu_p = {
	.attr.name = "modem_gard_mu_p",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_gard_start = {
	.attr.name = "modem_gard_start",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_gard_len_frz = {
	.attr.name = "modem_gard_len_frz",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

// added by moskvin 01.09.2017
static struct modem_fhss_attr modem_hopper_thcnt = {
	.attr.name = "modem_hopper_thcnt",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_hopper_slip = {
	.attr.name = "modem_hopper_slip",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_mlip_loop = {
	.attr.name = "modem_mlip_loop",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_mlip_cntcrc_on = {
	.attr.name = "modem_mlip_cntcrc_on",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_mlip_cntcrc = {
	.attr.name = "modem_mlip_cntcrc",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_mlip_cnt_tx = {
	.attr.name = "modem_mlip_cnt_tx",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_mlip_cnt_rx = {
	.attr.name = "modem_mlip_cnt_rx",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_gard_slip_fw = {
	.attr.name = "modem_gard_slip_fw",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct modem_fhss_attr modem_gard_slip_bw = {
	.attr.name = "modem_gard_slip_bw",
	.attr.mode = 0644,
	.value = 0,
	.priv = NULL,
};

static struct attribute * modem_fhss_attr[] = {
	&modem_rst.attr,
	&modem_loop.attr,
	&modem_look_det.attr,
	&modem_en_mod.attr,
	&modem_en_demod.attr,
	&modem_en_hopper.attr,
	&modem_sel_freq.attr,
	&modem_seed.attr,
	&modem_av_size.attr,
	&modem_dec_prd.attr,
	&modem_th_freq.attr,
	&modem_saw_gen.attr,
	&modem_lookup_size.attr,
	&modem_ad_en.attr,
	&modem_ad_txnrx.attr,
	&modem_ad_inc.attr,
	&modem_ad_dec.attr,
	&modem_ad_bust_conf.attr,
	&modem_ad_light.attr,
	&modem_cic_en.attr,
	&modem_dc_del_tx.attr,
	&modem_dc_del_rx.attr,
	&modem_afc_en.attr,
	&modem_afc_kp.attr,
	&modem_afc_ki.attr,
	&modem_gard_mu_p.attr,
	&modem_gard_start.attr,
	&modem_gard_len_frz.attr,
	&modem_hopper_thcnt.attr,
	&modem_hopper_slip.attr,
	&modem_mlip_loop.attr,
	&modem_mlip_cntcrc_on.attr,
	&modem_mlip_cntcrc.attr,
	&modem_mlip_cnt_tx.attr,
	&modem_mlip_cnt_rx.attr,
	&modem_gard_slip_bw.attr,
	&modem_gard_slip_fw.attr,
	NULL
};

static void chrdrv_set_attr_priv(struct chrdrv_private *priv)
{
	modem_rst.priv = priv;
	modem_loop.priv = priv;
	modem_look_det.priv = priv;
	modem_en_mod.priv = priv;
	modem_en_demod.priv = priv;
	modem_en_hopper.priv = priv;
	modem_sel_freq.priv = priv;
	modem_seed.priv = priv;
	modem_av_size.priv = priv;
	modem_dec_prd.priv = priv;
	modem_th_freq.priv = priv;
	modem_saw_gen.priv = priv;
	modem_lookup_size.priv = priv;
	modem_ad_en.priv = priv;
	modem_ad_txnrx.priv = priv;
	modem_ad_inc.priv = priv;
	modem_ad_dec.priv = priv;
	modem_ad_bust_conf.priv = priv;
	modem_ad_light.priv = priv;
	modem_cic_en.priv = priv;
	modem_dc_del_tx.priv = priv;
	modem_dc_del_rx.priv = priv;
	modem_afc_en.priv = priv;
	modem_afc_kp.priv = priv;
	modem_afc_ki.priv = priv;
	modem_gard_mu_p.priv = priv;
	modem_gard_start.priv = priv;
	modem_gard_len_frz.priv = priv;
	modem_hopper_thcnt.priv = priv;
	modem_hopper_slip.priv = priv;
	modem_mlip_loop.priv = priv;
	modem_mlip_cntcrc_on.priv = priv;
	modem_mlip_cntcrc.priv = priv;
	modem_mlip_cnt_tx.priv = priv;
	modem_mlip_cnt_rx.priv = priv;
	modem_gard_slip_bw.priv = priv;
	modem_gard_slip_fw.priv = priv;
}

#endif // _MODEM_OFDM_H_
