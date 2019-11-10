/**
 *  Файл: system_exception.h
 *
 *  Описание:
 */

#ifndef _SYSTEM_EXCEPTION_H_
#define _SYSTEM_EXCEPTION_H_

#include <exception>
#include "user_msg_parser.h"

class system_exception : public std::exception
{
public:
	system_exception(msg_type msg) noexcept;
	~system_exception() throw() {};

	/**
	 * @brief получение информации об исключении
	 *
	 * @return информация об ошибке
	 */
	const char* what() const noexcept override;

private:
	std::string msg_;
};

#endif //_SYSTEM_EXCEPTION_H_
