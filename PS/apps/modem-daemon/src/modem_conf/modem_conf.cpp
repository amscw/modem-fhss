#include "modem_conf.h"

#include <algorithm>

#include "access_log.h"

modem_conf::modem_conf()
{
	python_.py_set_module_path("/root");
	python_.py_import_module("modem_fhss_api");

	py_func_map_["modem_rst"] = modem_conf::py_func_num::MODEM_RST;
	py_func_map_["modem_loop"] = modem_conf::py_func_num::MODEM_LOOP;
	py_func_map_["modem_look_det"] = modem_conf::py_func_num::MODEM_LOOK_DET;
	py_func_map_["modem_en_mod"] = modem_conf::py_func_num::MODEM_EN_MOD;
	py_func_map_["modem_en_demod"] = modem_conf::py_func_num::MODEM_EN_DEMOD;
	py_func_map_["modem_en_hopper"] = modem_conf::py_func_num::MODEM_EN_HOPPER;
	py_func_map_["modem_sel_freq"] = modem_conf::py_func_num::MODEM_SEL_FREQ;
	py_func_map_["modem_seed"] = modem_conf::py_func_num::MODEM_SEED;
	py_func_map_["modem_av_size"] = modem_conf::py_func_num::MODEM_AV_SIZE;
	py_func_map_["modem_dec_prd"] = modem_conf::py_func_num::MODEM_DEC_PRD;
	py_func_map_["modem_th_freq"] = modem_conf::py_func_num::MODEM_TH_FREQ;
	py_func_map_["modem_saw_gen"] = modem_conf::py_func_num::MODEM_SAW_GEN;
	py_func_map_["modem_lookup_size"] = modem_conf::py_func_num::MODEM_LOOKUP_SIZE;
	py_func_map_["modem_ad_en"] = modem_conf::py_func_num::MODEM_AD_EN;
	py_func_map_["modem_ad_txnrx"] = modem_conf::py_func_num::MODEM_AD_TXNRX;
	py_func_map_["modem_ad_inc"] = modem_conf::py_func_num::MODEM_AD_INC;
	py_func_map_["modem_ad_dec"] = modem_conf::py_func_num::MODEM_AD_DEC;
	py_func_map_["modem_ad_bust_conf"] = modem_conf::py_func_num::MODEM_AD_BUST_CONF;
	py_func_map_["modem_ad_light"] = modem_conf::py_func_num::MODEM_AD_LIGHT;
	py_func_map_["modem_cic_en"] = modem_conf::py_func_num::MODEM_CIC_EN;
	py_func_map_["modem_dc_del_tx"] = modem_conf::py_func_num::MODEM_DC_DEL_TX;
	py_func_map_["modem_dc_del_rx"] = modem_conf::py_func_num::MODEM_DC_DEL_RX;
	py_func_map_["modem_afc_en"] = modem_conf::py_func_num::MODEM_AFC_EN;
	py_func_map_["modem_afc_kp"] = modem_conf::py_func_num::MODEM_AFC_KP;
	py_func_map_["modem_afc_ki"] = modem_conf::py_func_num::MODEM_AFC_KI;
	py_func_map_["modem_gard_mu_p"] = modem_conf::py_func_num::MODEM_GARD_MU_P;
	py_func_map_["modem_gard_start"] = modem_conf::py_func_num::MODEM_GARD_START;
	py_func_map_["modem_gard_len_frz"] = modem_conf::py_func_num::MODEM_GARD_LEN_FRZ;
	py_func_map_["modem_hopper_thcnt"] = modem_conf::py_func_num::MODEM_HOPPER_THCNT;
	py_func_map_["modem_hopper_slip"] = modem_conf::py_func_num::MODEM_HOPPER_SLIP;
	py_func_map_["modem_mlip_loop"] = modem_conf::py_func_num::MODEM_MLIP_LOOP;
	py_func_map_["modem_mlip_cntcrc_on"] = modem_conf::py_func_num::MODEM_MLIP_CNTCRC_ON;
	py_func_map_["modem_mlip_cntcrc"] = modem_conf::py_func_num::MODEM_MLIP_CNTCRC;
	py_func_map_["modem_mlip_cnt_tx"] = modem_conf::py_func_num::MODEM_MLIP_CNT_TX;
	py_func_map_["modem_mlip_cnt_rx"] = modem_conf::py_func_num::MODEM_MLIP_CNT_RX;
	py_func_map_["modem_gard_slip_fw"] = modem_conf::py_func_num::MODEM_GARD_SLIP_FW;
	py_func_map_["modem_gard_slip_bw"] = modem_conf::py_func_num::MODEM_GARD_SLIP_BW;
}

void modem_conf::run_conf(const std::vector<byte>& buffer, std::int64_t size) noexcept
{
	std::string buffer_str(buffer.begin(), buffer.begin() + size);
	std::vector<std::string> token = parser_msg(buffer_str);
	run_py_func(token);
}

std::vector<std::string> modem_conf::parser_msg(const std::string& buffer) noexcept
{
	std::string delimiter = " ";

	std::vector<std::string> token;
	size_t start_pos = 0;
	size_t current_pos = buffer.find(delimiter);

	while (current_pos != std::string::npos)
	{
		token.push_back(buffer.substr(start_pos, current_pos - start_pos));
		start_pos = current_pos + delimiter.length();
		current_pos = buffer.find(delimiter, start_pos);
	}
	token.push_back(buffer.substr(start_pos, current_pos));

	return token;
}

void modem_conf::run_py_func(const std::vector<std::string>& token) noexcept
{
	msg_result_.clear();

	if (token.at(0) == "set")
	{
		if (token.size() != 3)
		{
			msg_result_ = "Not valid number arguments";
			return;
		}

		if (!is_digits(token.at(2)))
		{
			msg_result_ = "Argument value is not digit";
			return;
		}

		switch(py_func_map_[token.at(1)])
		{
			case modem_conf::py_func_num::MODEM_RST:
			{
				python_.py_function_call("setModemRst", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemRst success";
				break;
			}
			case modem_conf::py_func_num::MODEM_LOOP:
			{
				python_.py_function_call("setModemLoop", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemLoop success";
				break;
			}
			//case modem_conf::py_func_num::MODEM_LOOK_DET:
			//{
			//	python_.py_function_call("setModemLookDet", std::move(std::stoi(token.at(2))));
			//	msg_result_ = "setModemLookDet success";
			//	break;
			//}
			case modem_conf::py_func_num::MODEM_EN_MOD:
			{
				python_.py_function_call("setModemEnMod", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemEnMod success";
				break;
			}
			case modem_conf::py_func_num::MODEM_EN_DEMOD:
			{
				python_.py_function_call("setModemEnDemod", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemEnDemod success";
				break;
			}
			case modem_conf::py_func_num::MODEM_EN_HOPPER:
			{
				python_.py_function_call("setModemEnHopper", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemEnHopper success";
				break;
			}
			case modem_conf::py_func_num::MODEM_SEL_FREQ:
			{
				python_.py_function_call("setModemSelFreq", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemSelFreq success";
				break;
			}
			case modem_conf::py_func_num::MODEM_SEED:
			{
				python_.py_function_call("setModemSeed", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemSeed success";
				break;
			}
			case modem_conf::py_func_num::MODEM_AV_SIZE:
			{
				python_.py_function_call("setModemAvSize", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemAvSize success";
				break;
			}
			case modem_conf::py_func_num::MODEM_DEC_PRD:
			{
				python_.py_function_call("setModemDecPrd", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemDecPrd success";
				break;
			}
			case modem_conf::py_func_num::MODEM_TH_FREQ:
			{
				python_.py_function_call("setModemThFreq", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemThFreq success";
				break;
			}
			case modem_conf::py_func_num::MODEM_SAW_GEN:
			{
				python_.py_function_call("setModemSawGen", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemSawGen success";
				break;
			}
			case modem_conf::py_func_num::MODEM_LOOKUP_SIZE:
			{
				python_.py_function_call("setModemLookupSize", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemLookupSize success";
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_EN:
			{
				python_.py_function_call("setModemAdEn", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemAdEn success";
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_TXNRX:
			{
				python_.py_function_call("setModemAdTxnrx", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemAdTxnrx success";
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_INC:
			{
				python_.py_function_call("setModemAdInc", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemAdInc success";
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_DEC:
			{
				python_.py_function_call("setModemAdDec", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemAdDec success";
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_BUST_CONF:
			{
				python_.py_function_call("setModemAdBustConf", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemAdBustConf success";
				break;
			}
			//case modem_conf::py_func_num::MODEM_AD_LIGHT:
			//{
			//	python_.py_function_call("setModemAdLight", std::move(std::stoi(token.at(2))));
			//	msg_result_ = "setModemAdLight success";
			//	break;
			//}
			case modem_conf::py_func_num::MODEM_CIC_EN:
			{
				python_.py_function_call("setModemCicEn", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemCicEn success";
				break;
			}
			case modem_conf::py_func_num::MODEM_DC_DEL_TX:
			{
				python_.py_function_call("setModemDcDelTx", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemDcDelTx success";
				break;
			}
			case modem_conf::py_func_num::MODEM_DC_DEL_RX:
			{
				python_.py_function_call("setModemDcDelRx", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemDcDelRx success";
				break;
			}
			case modem_conf::py_func_num::MODEM_AFC_EN:
			{
				python_.py_function_call("setModemAfcEn", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemAfcEn success";
				break;
			}
			case modem_conf::py_func_num::MODEM_AFC_KP:
			{
				python_.py_function_call("setModemAfcKp", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemAfcKp success";
				break;
			}
			case modem_conf::py_func_num::MODEM_AFC_KI:
			{
				python_.py_function_call("setModemAfcKi", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemAfcKi success";
				break;
			}
			case modem_conf::py_func_num::MODEM_GARD_MU_P:
			{
				python_.py_function_call("setModemGardMuP", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemGardMuP success";
				break;
			}
			case modem_conf::py_func_num::MODEM_GARD_START:
			{
				python_.py_function_call("setModemGardStart", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemGardStart success";
				break;
			}
			case modem_conf::py_func_num::MODEM_GARD_LEN_FRZ:
			{
				python_.py_function_call("setModemGardLenFrz", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemGardLenFrz success";
				break;
			}
			case modem_conf::py_func_num::MODEM_HOPPER_THCNT:
			{
				python_.py_function_call("setModemHopperThcnt", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemHopperThcnt success";
				break;				
			}
			case modem_conf::py_func_num::MODEM_HOPPER_SLIP:
			{
				python_.py_function_call("setModemHopperSlip", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemHopperSlip success";
				break;				
			}
			case modem_conf::py_func_num::MODEM_MLIP_LOOP:
			{
				python_.py_function_call("setModemMlipLoop", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemMlipLoop success";
				break;				
			}
			case modem_conf::py_func_num::MODEM_MLIP_CNTCRC_ON:
			{
				python_.py_function_call("setModemMlipCntcrcOn", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemMlipCntcrcOn success";
				break;				
			}
			// case modem_conf::py_func_num::MODEM_MLIP_CNTCRC:
			// {
			// 	python_.py_function_call("setModemMlipCntcrc", std::move(std::stoi(token.at(2))));
			// 	msg_result_ = "setModemMlipCntcrc success";
			// 	break;				
			// }
			// case modem_conf::py_func_num::MODEM_MLIP_CNT_TX:
			// {
			// 	python_.py_function_call("setModemMlipCntTx", std::move(std::stoi(token.at(2))));
			// 	msg_result_ = "setModemMlipCntTx success";
			// 	break;				
			// }
			// case modem_conf::py_func_num::MODEM_MLIP_CNT_RX:
			// {
			// 	python_.py_function_call("setModemMlipCntRx", std::move(std::stoi(token.at(2))));
			// 	msg_result_ = "setModemMlipCntRx success";
			// 	break;				
			// }
			case modem_conf::py_func_num::MODEM_GARD_SLIP_FW:
			{
				python_.py_function_call("setModemGardSlipFw", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemGardSlipFw success";				
				break;
			}
			case modem_conf::py_func_num::MODEM_GARD_SLIP_BW:
			{
				python_.py_function_call("setModemGardSlipBw", std::move(std::stoi(token.at(2))));
				msg_result_ = "setModemGardSlipBw success";				
				break;
			}			
			default:
			{
				msg_result_ = "Register not valid";
				break;
			}
		}
	}
	else if (token.at(0) == "get")
	{
		if (token.size() != 2)
		{
			msg_result_ = "Not valid number arguments";
			return;
		}

		switch(py_func_map_[token.at(1)])
		{
			case modem_conf::py_func_num::MODEM_RST:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemRst");
				msg_result_ = "modem_rst " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_LOOP:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemLoop");
				msg_result_ = "modem_loop " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_LOOK_DET:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemLookDet");
				msg_result_ = "modem_look_det " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_EN_MOD:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemEnMod");
				msg_result_ = "modem_en_mod " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_EN_DEMOD:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemEnDemod");
				msg_result_ = "modem_en_demod " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_EN_HOPPER:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemEnHopper");
				msg_result_ = "modem_en_hopper " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_SEL_FREQ:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemSelFreq");
				msg_result_ = "modem_sel_freq " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_SEED:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemSeed");
				msg_result_ = "modem_seed " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AV_SIZE:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAvSize");
				msg_result_ = "modem_av_size " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_DEC_PRD:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemDecPrd");
				msg_result_ = "modem_dec_prd " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_TH_FREQ:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemThFreq");
				msg_result_ = "modem_th_freq " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_SAW_GEN:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemSawGen");
				msg_result_ = "modem_saw_gen " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_LOOKUP_SIZE:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemLookupSize");
				msg_result_ = "modem_lookup_size " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_EN:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAdEn");
				msg_result_ = "modem_ad_en " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_TXNRX:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAdTxnrx");
				msg_result_ = "modem_ad_txnrx " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_INC:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAdInc");
				msg_result_ = "modem_ad_inc " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_DEC:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAdDec");
				msg_result_ = "modem_ad_dec " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_BUST_CONF:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAdBustConf");
				msg_result_ = "modem_ad_bust_conf " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AD_LIGHT:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAdLight");
				msg_result_ = "modem_ad_light " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_CIC_EN:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemCicEn");
				msg_result_ = "modem_cic_en " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_DC_DEL_TX:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemDcDelTx");
				msg_result_ = "modem_dc_del_tx " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_DC_DEL_RX:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemDcDelRx");
				msg_result_ = "modem_dc_del_rx " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AFC_EN:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAfcEn");
				msg_result_ = "modem_afc_en " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AFC_KP:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAfcKp");
				msg_result_ = "modem_afc_kp " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_AFC_KI:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemAfcKi");
				msg_result_ = "modem_afc_ki " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_GARD_MU_P:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemGardMuP");
				msg_result_ = "modem_gard_mu_p " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_GARD_START:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemGardStart");
				msg_result_ = "modem_gard_start " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_GARD_LEN_FRZ:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemGardLenFrz");
				msg_result_ = "modem_gard_len_frz " + std::to_string(val);
				break;
			}

			case modem_conf::py_func_num::MODEM_HOPPER_THCNT:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemHopperThcnt");
				msg_result_ = "modem_hopper_thcnt " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_HOPPER_SLIP:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemHopperSlip");
				msg_result_ = "modem_hopper_slip " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_MLIP_LOOP:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemMlipLoop");
				msg_result_ = "modem_mlip_loop " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_MLIP_CNTCRC_ON:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemMlipCntcrcOn");
				msg_result_ = "modem_mlip_cntcrc_on " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_MLIP_CNTCRC:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemMlipCntcrc");
				msg_result_ = "modem_mlip_cntcrc " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_MLIP_CNT_TX:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemMlipCntTx");
				msg_result_ = "modem_mlip_cnt_tx " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_MLIP_CNT_RX:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemMlipCntRx");
				msg_result_ = "modem_mlip_cnt_rx " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_GARD_SLIP_FW:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemGardSlipFw");
				msg_result_ = "modem_gard_slip_fw " + std::to_string(val);
				break;
			}
			case modem_conf::py_func_num::MODEM_GARD_SLIP_BW:
			{
				std::uint32_t val = python_.py_ret_function_call<std::uint32_t>("getModemGardSlipBw");
				msg_result_ = "modem_gard_slip_bw " + std::to_string(val);
				break;
			}
			default:
			{
				msg_result_ = "Register not valid";
				break;
			}
		}
	}
	else if (token.at(0) == "send")
	{
		std::string data;
		for (int i = 1; i < token.size(); i++)
		{
			data = data + token.at(i) + " ";
		}
		if (!data.empty())
		{
			msg_result_ = python_.py_ret_function_call<std::string>("modemSendData", std::move(data));
			if (msg_result_.empty())
			{
				msg_result_ = "Message not receive";
			}
		}
		else
		{
			msg_result_ = "message is empty";
		}
	}
	else
	{
		msg_result_ = "First argument must be set/get/send";
	}
}

bool modem_conf::is_digits(const std::string& str) const noexcept
{
	return std::all_of(str.begin(), str.end(), ::isdigit);
}

std::string modem_conf::get_msg_result() noexcept
{
	return msg_result_;
}
