/**
 * Флайл: process_creator.h
 *
 * Описание: абстрактный создатель нового процесса
 */

#ifndef _PROCESS_CREATOR_H_
#define _PROCESS_CREATOR_H_

#include "process.h"

#include <memory>
#include <unistd.h>

class process_creator
{
public:
	process_creator() = default;
	virtual ~process_creator() = default;

	/**
	 * @brief создатель нового процесса
	 *
	 * @retun pid созданного процесса
	 */
	virtual pid_t create_process() noexcept = 0;

	/**
	 * @brief создание экземпляра класса process
	 *
	 * @return новый экземпляр класса process
	 */
	virtual std::unique_ptr<process> get_process() noexcept = 0;

	process_creator(const process_creator& copy) = delete;
	process_creator & operator=(const process_creator& copy) = delete;

	enum class state_process : pid_t {CHILD_PROCESS = 0, ERROR_PROCESS = -1};
};

#endif //_PROCESS_CREATOR_H_
