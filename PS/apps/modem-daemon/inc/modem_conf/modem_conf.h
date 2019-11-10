/**
 * Флайл: modem_conf.h
 *
 * Описание: конфигурация модема
 */

#ifndef _MODEM_CONF_H_
#define _MODEM_CONF_H_

#include "types.h"

#include <map>
#include <python_c_api.h>
#include <string>
#include <vector>

class modem_conf
{
public:
	modem_conf();
	~modem_conf() = default;

	/**
	 * @brief запуск конфигурации
	 */
	void run_conf(const std::vector<byte>& buffer, std::int64_t size) noexcept;

	/**
	 * @brief результат выполнения
	 */
	std::string get_msg_result() noexcept;

	modem_conf(const modem_conf& copy) = delete;
	modem_conf& operator=(const modem_conf& copy) = delete;

private:
	/**
	 * @brief парсинг входящего сообщения
	 */
	std::vector<std::string> parser_msg(const std::string& buffer) noexcept;

	/**
	 * @brief запуск метода python
	 */
	void run_py_func(const std::vector<std::string>& v) noexcept;

	/**
	 * @brief проверка строки на содердание числа
	 */
	bool is_digits(const std::string& str) const noexcept;

private:
	//
	python_c_api python_;

	//
	std::string msg_result_;

	//
	enum class py_func_num : std::uint32_t { MODEM_RST = 1,
											MODEM_LOOP,
											MODEM_LOOK_DET,
											MODEM_EN_MOD,
											MODEM_EN_DEMOD,
											MODEM_EN_HOPPER,
											MODEM_SEL_FREQ,
											MODEM_SEED,
											MODEM_AV_SIZE,
											MODEM_DEC_PRD,
											MODEM_TH_FREQ,
											MODEM_SAW_GEN,
											MODEM_LOOKUP_SIZE,
											MODEM_AD_EN,
											MODEM_AD_TXNRX,
											MODEM_AD_INC,
											MODEM_AD_DEC,
											MODEM_AD_BUST_CONF,
											MODEM_AD_LIGHT,
											MODEM_CIC_EN,
											MODEM_DC_DEL_TX,
											MODEM_DC_DEL_RX,
											MODEM_AFC_EN,
											MODEM_AFC_KP,
											MODEM_AFC_KI,
											MODEM_GARD_MU_P,
											MODEM_GARD_START,
											MODEM_GARD_LEN_FRZ,
											MODEM_HOPPER_THCNT,
											MODEM_HOPPER_SLIP,
											MODEM_MLIP_LOOP,
											MODEM_MLIP_CNTCRC_ON,
											MODEM_MLIP_CNTCRC,
											MODEM_MLIP_CNT_TX,
											MODEM_MLIP_CNT_RX,
											MODEM_GARD_SLIP_FW,
											MODEM_GARD_SLIP_BW };


	//
	std::map <std::string, py_func_num> py_func_map_;
};

#endif //_MODEM_CONF_H_
