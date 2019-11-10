/**
 * Флайл: worker_process_creator.h
 *
 * Описание: создатель мастер процесса
 */

#ifndef _WORKER_PROCESS_CREATOR_H_
#define _WORKER_PROCESS_CREATOR_H_

#include "process_creator.h"

class worker_process_creator : public process_creator
{
public:
	worker_process_creator() = default;
	~worker_process_creator() = default;

	/**
	 * @brief создание нового процесса
	 *
	 * @return pid созданного процесса
	 */
	pid_t create_process() noexcept override;

	/**
	 * @brief получение worker-процесса
	 *
	 * @return std::unique_ptr<process> указатель на созданный объект worker-процесса
	 */
	std::unique_ptr<process> get_process() noexcept override;
};

#endif //_WORKER_PROCESS_CREATOR_H_
