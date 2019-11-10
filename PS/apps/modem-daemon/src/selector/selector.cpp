#include "selector.h"

#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

const int selector::max_events_ = 32;
const int selector::epoll_timeout_ = 10;

int selector::register_connect(std::unique_ptr<connection> tcp) noexcept
{
	int socket = tcp->get_slave_socket();
	set_nonblock(socket);

	struct epoll_event event;
	event.data.fd = socket;
	event.events = EPOLLIN;
	epoll_ctl(epoll_, EPOLL_CTL_ADD, socket, &event);

	conn_.insert(std::pair<int, std::unique_ptr<connection>>(socket, std::move(tcp)));
	return socket;
}

void selector::wait_event() noexcept
{
	struct epoll_event events[max_events_];
	int count_events = epoll_wait(epoll_, events, max_events_, epoll_timeout_);	// ждет события 10 мс
	for (int i = 0; i < count_events; i++)
	{
		std::unique_ptr<connection>& tcp = conn_.find(events[i].data.fd)->second;
		epoll_data_ready_->emit(tcp);
	}
}

void selector::connection_delete(int slave_socket) noexcept
{
	conn_.erase(slave_socket);
}

int selector::set_nonblock(const int &fd) noexcept
{
	int res = default_error_code;

	res = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

	return res;
}

std::unique_ptr<connection> &selector::tcp(int socket_id) 
{
	try
	{
		return conn_.at(socket_id);
	} catch (std::out_of_range) { throw; }
}
