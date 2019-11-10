#include "worker_process.h"
#include "worker_process_creator.h"

#include <unistd.h>

pid_t worker_process_creator::create_process() noexcept
{
	return fork();
}
std::unique_ptr<process> worker_process_creator::get_process() noexcept
{
	return std::make_unique<worker_process>();
}
