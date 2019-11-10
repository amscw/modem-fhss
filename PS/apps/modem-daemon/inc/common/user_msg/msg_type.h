/**
 *   Файл: msg_type.h
 *
 *   Описание: перечисление пользовательских сообщений
 *
 */

#ifndef _MSG_TYPE_H_
#define _MSG_TYPE_H_

enum msg_type
{
	msg_system_error = 0,
	msg_start_daemon_error,
	msg_init_access_log_err,
	msg_init_error_log_err,

	msg_end //последний элемент перечисления
};
#endif //_MSG_H_
