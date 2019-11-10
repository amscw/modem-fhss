// #include "endpoint_ipv6.h"
//
// #include <arpa/inet.h>
// #include <array>
// #include <cassert>
// #include <cstring>
// #include <types.h>
//
// endpoint_ipv6::endpoint_ipv6()
// {
// 	std::memset(&sock_address6_, 0x00, sizeof(sock_address6_));
// }
//
// endpoint_ipv6::endpoint_ipv6(const std::uint16_t port)
// {
// 	std::memset(&sock_address6_, 0x00, sizeof(sock_address6_));
// 	sock_address6_.sin6_flowinfo = 0;	// ?
// 	sock_address6_.sin6_family = AF_INET6;
// 	sock_address6_.sin6_addr = in6addr_any;
// 	sock_address6_.sin6_port = htons(port);
// 	sock_address6_.sin6_scope_id = 0;	// ?
// }
//
// endpoint_ipv6::endpoint_ipv6(const std::string &ip_address, std::uint16_t port)
// {
// 	assert(!ip_address.empty());
//
// 	std::memset(&sock_address6_, 0x00, sizeof(sock_address6_));
// 	sock_address6_.sin6_flowinfo = 0;	// ?
// 	sock_address6_.sin6_family = AF_INET6;
// 	inet_pton(AF_INET6, ip_address.c_str(), &sock_address6_.sin6_addr);
// 	sock_address6_.sin6_port = htons(port);
// 	sock_address6_.sin6_scope_id = 0;	// ?
// }
//
// std::string endpoint_ipv6::get_ip_address() const noexcept
// {
// 	std::array<char, INET6_ADDRSTRLEN + 1> temp_buff;
// 	temp_buff[INET6_ADDRSTRLEN] = '\0';
// 	std::memset(temp_buff.data(), 0x00, INET_ADDRSTRLEN);
// 	inet_ntop(AF_INET, &sock_address6_.sin6_addr, temp_buff.data(), INET6_ADDRSTRLEN);
// 	return std::string(temp_buff.data());
// }
//
// std::uint16_t endpoint_ipv6::get_port() const noexcept
// {
// 	uint16_t res = 0;
// 	res = htons(sock_address6_.sin6_port);
// 	return res;
// }
//
// int endpoint_ipv6::get_domain() const noexcept
// {
// 	return AF_INET6;
// }
//
// int endpoint_ipv6::get_type() const noexcept
// {
// 	return SOCK_STREAM;
// }
//
// int endpoint_ipv6::get_protocol() const noexcept
// {
// 	return IPPROTO_TCP;
// }
//
// struct sockaddr *endpoint_ipv6::get_sockaddr() const noexcept
// {
// 	return (struct sockaddr *)&sock_address6_;
// }
//
// socklen_t endpoint_ipv6::get_sockaddr_size() const noexcept
// {
// 	return sizeof(struct sockaddr_in6);
// }
//
// int endpoint_ipv6::get_backlong() const noexcept
// {
// 	return SOMAXCONN;
// }
//
// endpoint_ipv6::endpoint_ipv6(const endpoint_ipv6 &copy)
// {
// 	std::memcpy(&sock_address6_, &copy.sock_address6_, sizeof(sock_address6_));
// }
//
// endpoint_ipv6 &endpoint_ipv6::operator=(const endpoint_ipv6 &copy)
// {
// 	if (this != &copy)
// 	{
// 		std::memcpy(&sock_address6_, &copy.sock_address6_, sizeof(sock_address6_));
// 	}
// 	return *this;
// }
//
// endpoint_ipv6::endpoint_ipv6(endpoint_ipv6 &&other)
// {
// 	std::memcpy(&sock_address6_, &other.sock_address6_, sizeof(sock_address6_));
// 	std::memset(&other.sock_address6_, 0x00, sizeof(other.sock_address6_));
// }
//
// endpoint_ipv6 &endpoint_ipv6::operator=(endpoint_ipv6 &&other)
// {
// 	std::swap(sock_address6_, other.sock_address6_);
// 	return *this;
// }
