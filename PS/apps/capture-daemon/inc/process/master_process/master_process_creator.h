/**
 * Флайл: master_process_creator.h
 *
 * Описание: создатель мастер-процесса
*/

#ifndef _MASTER_PROCESS_CREATOR_H_
#define _MASTER_PROCESS_CREATOR_H_

#include "process_creator.h"

class master_process_creator : public process_creator
{
public:
	master_process_creator() = default;
	~master_process_creator() = default;

	/**
	 * @brief создание нового процесса
	 *
	 * @return pid созданного процесса
	 */
	pid_t create_process() noexcept override;

	/**
	 * @brief получение мастер-процесса
	 *
	 * @return std::unique_ptr<process> указатель на созданный объект мастер-процесса
	 */
	std::unique_ptr<process> get_process() noexcept override;

	master_process_creator(const master_process_creator& copy) = delete;
	master_process_creator& operator=(const master_process_creator& copy) = delete;
};

#endif //_MASTER_PROCESS_CREATOR_H_
