/**
 * Флайл: process.h
 *
 * Описание: абстрактный процесс
 */

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <memory>

class process
{
public:
	process() = default;
	virtual ~process() = default;

	/**
	 * @brief точка входа в новый процесс
	 */
	virtual void start_process() = 0;

	process(const process& copy) = delete;
	process& operator=(const process& copy) = delete;
};

#endif //_PROCESS_H_
