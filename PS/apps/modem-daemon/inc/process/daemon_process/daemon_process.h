/**
 * Флайл: daemon_process.h
 *
 * Описание: создание процесса-демона
*/

#ifndef _DAEMON_PROCESS_H_
#define _DAEMON_PROCESS_H_

#include "process.h"

#include <memory>

class daemon_process : public process
{
public:
	daemon_process();
	~daemon_process();

	/**
	 * @brief запуск процесса-демона
	 */
	void start_process() noexcept override;

	daemon_process(const daemon_process& copy) = delete;
	daemon_process & operator=(const daemon_process& copy) = delete;

private:
	class impl;
	std::unique_ptr<impl> pimpl_;
};

#endif //_DAEMON_PROCESS_H_
