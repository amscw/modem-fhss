#include "master_process.h"
#include "master_process_creator.h"

#include <unistd.h>

pid_t master_process_creator::create_process() noexcept
{
	return fork();
}

std::unique_ptr<process> master_process_creator::get_process() noexcept
{
	return std::make_unique<master_process>();
}
