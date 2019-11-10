/**
 * Флайл: modem_conf.h
 *
 * Описание: конфигурация модема
 */

#ifndef _MODEM_CONF_H_
#define _MODEM_CONF_H_

#include "types.h"

#include <string>
#include <vector>

class modem_conf
{
public:
	modem_conf() = default;
	~modem_conf() = default;

	/**
	 * @brief запуск конфигурации
	 */
	std::vector<std::string> run(const std::vector<byte>& buffer) noexcept;

	modem_conf(const modem_conf& copy) = delete;
	modem_conf& operator=(const modem_conf& copy) = delete;
};

#endif //_MODEM_CONF_H_
