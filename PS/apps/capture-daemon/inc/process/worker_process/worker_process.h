/**
 * Флайл: worker_process.h
 *
 * Описание: класс реализющий worker процесс
 */

#ifndef _WORKER_PROCESS_H_
#define _WORKER_PROCESS_H_

#include "types.h"
#include "process.h"
#include "tcp_connection_creator.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#define IOC_MAGIC       1
#define IOC_MAXNR       3
#define IOC_SET_SADDR	_IOW(IOC_MAGIC, 1, unsigned int)
#define IOC_SET_CFLAGS	_IOW(IOC_MAGIC, 2, unsigned int)

class worker_process : public process
{
public:
	struct cappkt
	{
		std::uint8_t header;
		std::uint8_t flags;
		std::uint8_t address;
		std::uint16_t length;
	}__attribute__((packed));

public:
	worker_process() : tcp_create_(nullptr), cp_{0, 0, 0, 0}, capture_(nullptr) {};
	~worker_process() = default;

	/**
	 * @brief получение соединения
	 */
	void get_connection(std::unique_ptr<tcp_connection_creator> tcp_create) noexcept;

	/**
	 * @brief точка входа в worker процесс
	 */
	void start_process() noexcept override;

	worker_process(const worker_process& copy) = delete;
	worker_process& operator=(const worker_process& copy) = delete;

private:
	char * capture_;
	struct cappkt cp_;
	std::unique_ptr<tcp_connection_creator> tcp_create_;
};

#endif //_WORKER_H_
