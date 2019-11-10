/**
 *   Файл: user_msg_parser.h
 *
 *   Описание: реализация интерфейса доступа к пользовательским сообщениям
 *
 */

#ifndef _USER_MSG_PARSER_H_
#define _USER_MSG_PARSER_H_

#include <string>

#include "msg_type.h"
#include "destroyer_singleton.h"

class user_msg_parser
{
public:
	user_msg_parser() = default;
	~user_msg_parser() = default;
	/**
	 *   @brief инстансирование единосвенного экземпляра класса singleton
	 *
	 *   @return указатель на единственный экземпляр класса user_msg_parser
	 */
	static user_msg_parser* get_instance() noexcept;

	/**
	 *   @brief метод поиска пользовательского сообщения по интетификатору
	 *
	 *   @return пользовательское сообщение
	 */
	const std::string& find_user_msg(msg_type msg) const noexcept;

    user_msg_parser(const user_msg_parser& copy) = delete;
    user_msg_parser& operator=(const user_msg_parser& copy) = delete;

private:
	static user_msg_parser* instance_;
	static destroyer_singleton<user_msg_parser> destroyer_;

	friend class destroyer_singleton<user_msg_parser>;

	static const std::string user_msg_[];
};

#endif //_USER_MSG_PARSER_H_
