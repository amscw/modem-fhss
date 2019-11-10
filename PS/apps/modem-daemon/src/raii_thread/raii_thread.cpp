#include "raii_thread.h"

raii_thread::~raii_thread() noexcept
{
	if (thread_.joinable())
	{
		cancel();
	}
}

void raii_thread::cancel() noexcept
{
	token_.cancel();
	thread_.join();
}
