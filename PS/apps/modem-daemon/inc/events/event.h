#ifndef _EVENT_H_
#define _EVENT_H_

#include "event_traits.h"

#include <functional>
#include <map>
#include <memory>

template <typename... Args>
class event
{
public:
	using function = std::function<void(Args...)>;

	event();

	~event();

	event_id connect(function const&) const noexcept;

	void disconnect(event_id) const noexcept;

	void emit(Args... args) noexcept;

	event(const event&);

	event& operator=(const event&);

	event(event&&);

	event& operator=(event&&);
private:
	using event_table = std::map<event_id, function>;

	mutable event_id current_id_ { 0 };
	mutable event_table table_;
};

template<>
class event<void>
{
public:
	using void_function = std::function<void()>;

	event();

	~event();

	event_id connect(void_function const&) const noexcept;

	void disconnect(event_id) const noexcept;

	void emit() noexcept;

	event(const event&) = delete;

	event& operator=(event&) = delete;

	event(event&&) = default;

	event& operator=(event&&) = default;

private:
	struct impl;
	std::unique_ptr<impl> d_;
};


#endif //_EVENT_H
