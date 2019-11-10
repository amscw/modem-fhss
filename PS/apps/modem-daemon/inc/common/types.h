/**
 *  Файл: types.h
 *
 *  Описание: Файл для объявления общих типов и структур
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#include <string>

typedef unsigned char byte;

static int default_error_code = -1;

static const std::uint32_t tcp_port_max = 65535;

static const std::string sync_server_name = "synchronous";
static const std::string async_server_name = "asynchronous";

#endif //_TYPES_H_
