#include "access_log.h"
#include "connection_creator.h"
#include "connection_except.h"
#include "error_log.h"
#include "server_config.h"
#include "worker_process.h"

#include <future>
#include <unistd.h>

void worker_process::get_connection(std::unique_ptr<tcp_connection_creator> tcp_create) noexcept
{
	assert(tcp_create != nullptr);
	tcp_create_ = std::move(tcp_create);
}

void worker_process::start_process() noexcept
{
	while(1)
	{
		assert(tcp_create_ != nullptr);
		std::unique_ptr<connection> tcp = tcp_create_->get_connection();

		access_log::get_instance()->save_log("Connect mathlab client");

		std::int64_t ret = tcp->receive_data((char *)&cp_, sizeof(struct cappkt));

		if (cp_.header == server_config::get_instance()->get_config()->capture_header_)
		{
			std::string capture_devname = server_config::get_instance()->get_config()->capture_devname_;
			std::size_t fd = open(capture_devname.c_str(), O_RDWR);
			if (fd < 0)
			{
				error_log::get_instance()->save_log("Open fail " + capture_devname);
				close(tcp->get_slave_socket());
				break;
			}

			std::uint32_t addr = 0;
			std::uint32_t flags = 0;
			std::size_t length = 0;

			addr = (std::uint32_t)cp_.address;
			flags = (std::uint32_t)cp_.flags;
			length = (std::size_t)cp_.length;

			ioctl(fd, IOC_SET_SADDR, &addr);
			ioctl(fd, IOC_SET_CFLAGS, &flags);

			capture_ = (char *)malloc(length*4);

			ret = read(fd, capture_, length*4);

			tcp->send_data(capture_, length*4);

			free(capture_);
			close(fd);
		}
		if (tcp->get_slave_socket())
		{
			close(tcp->get_slave_socket());
		}

		access_log::get_instance()->save_log("Client mathlab disconnect");
	}
}
