#include "tcp_connection.h"

#include <cassert>
#include <unistd.h>

tcp_connection::tcp_connection(int slave_socket) : slave_socket_(slave_socket)
{

}

tcp_connection::~tcp_connection()
{
	if (slave_socket_ > 0)
	{
		//close(slave_socket_);
	}
}

int tcp_connection::get_slave_socket() const noexcept
{
	return slave_socket_;
}

std::int64_t tcp_connection::send_data(const std::string &buffer) const noexcept
{
	assert(!buffer.empty());
	return send(slave_socket_, &buffer[0], buffer.length(), MSG_NOSIGNAL);
}

std::int64_t tcp_connection::send_data(const std::vector<byte> &buffer) const noexcept
{
	assert(!buffer.empty());
	return send(slave_socket_, &buffer[0], buffer.size(), MSG_NOSIGNAL);
}

std::int64_t tcp_connection::receive_data(std::string &buffer) const noexcept
{
	return recv(slave_socket_, &buffer[0], buffer.length(), MSG_NOSIGNAL);
}

std::int64_t tcp_connection::receive_data(std::vector<byte> &buffer) const noexcept
{
	return recv(slave_socket_, &buffer[0], buffer.size(), MSG_NOSIGNAL);
}

std::uint16_t tcp_connection::get_slave_port() const
{
	struct sockaddr_in peeraddr;
	socklen_t peeraddrlen = sizeof (peeraddr);
	getpeername(slave_socket_, (struct sockaddr*)&peeraddr, &peeraddrlen);
	return ntohs(peeraddr.sin_port);
}
