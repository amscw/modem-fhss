/**
 * Флайл: master_process.h
 *
 * Описание: создание мастер-процесса
*/

#ifndef _MASTER_PROCESS_H_
#define _MASTER_PROCESS_H_

#include "process.h"

#include <memory>

class master_process : public process
{
public:
	master_process();
	~master_process();

	/**
	 * @brief запуск мастер-процесса
	 */
	void start_process() noexcept override;

	master_process(const master_process& copy) = delete;
	master_process& operator=(const master_process& copy) = delete;

private:
	class impl;
	std::unique_ptr<impl> pimpl_;
};

#endif //_MASTER_PROCESS_H_
