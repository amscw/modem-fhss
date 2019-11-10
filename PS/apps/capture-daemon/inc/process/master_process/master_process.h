/**
 * Флайл: master_process.h
 *
 * Описание: создание мастер-процесса
*/

#ifndef _MASTER_PROCESS_H_
#define _MASTER_PROCESS_H_


#include "process.h"
#include <memory>
#include <signal.h>

class master_process : public process
{
public:
	master_process() = default;
	~master_process() = default;

	/**
	 * @brief запуск мастер-процесса
	 */
	void start_process() noexcept override;

	master_process(const master_process& copy) = delete;
	master_process& operator=(const master_process& copy) = delete;

private:
	/**
	 * @brief регистрация сигналов обработчиком сигналов
	 *
	 * @param sigset набор сигналов
	 * @param siginfo информация о сигналах
	 */
	void setup_signal(sigset_t& sigset, siginfo_t& siginfo) noexcept;
};

#endif //_MASTER_PROCESS_H_
