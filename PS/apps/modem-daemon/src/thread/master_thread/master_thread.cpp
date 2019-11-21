#include "connection_except.h"
#include "endpoint_ipv4.h"
#include "error_log.h"
#include "master_thread.h"
#include "raii_thread.h"
#include "tcp_connection_creator.h"
#include "threadsafe_queue.h"

#include <unistd.h>
#include <functional>

void master_thread::run(cancellation_token& token, std::unique_ptr<threadsafe_queue<std::unique_ptr<connection>>> &queue,
  std::unique_ptr<endpoint_ipv4> &ep) noexcept
{
	std::unique_ptr<tcp_connection_creator> tcp_create = nullptr;
	try
	{
		// Endpoint перемещается в созданный объект tcp_create
		tcp_create = std::make_unique<tcp_connection_creator>(std::move(ep));
	}
	catch (connection_except &ex)
	{
		error_log::get_instance()->save_log(ex.what());
		return;
	}

	while (token)
	{
		std::unique_ptr<connection> tcp;
		try
		{
			// Получение соединения
			tcp = tcp_create->get_connection();
		}
		catch (connection_except& ex)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}

		// Отправка полученного соединения в очередь
		queue->push(std::move(tcp));
	}
}
