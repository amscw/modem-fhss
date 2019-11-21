#include "connection.h"
#include "master_thread.h"
#include "raii_thread.h"
#include "tcp_server_async.h"
#include "threadsafe_queue.h"
#include "worker_thread.h"

class tcp_server_async::impl
{
public:
	impl() : queue_(nullptr), master_thread_(nullptr), worker_thread_(nullptr), master_thread_creator_(nullptr),
	  worker_thread_creator_(nullptr) {};

	~impl() = default;

public:
	// Очередь сообщений между master_thread и worker_thread
	std::unique_ptr<threadsafe_queue<std::unique_ptr<connection>>> queue_;

	// Тело master_thread
	std::unique_ptr<master_thread> master_thread_;

	// Тело worker_thread
	std::unique_ptr<worker_thread> worker_thread_;

	// Создатель master_thread
	std::unique_ptr<raii_thread> master_thread_creator_;

	// Создатель worker_thread
	std::unique_ptr<raii_thread> worker_thread_creator_;
};

tcp_server_async::tcp_server_async() : stop_master_threads_(std::make_unique<event<void>>()),
  stop_worker_threads_(std::make_unique<event<void>>()), pimpl_(std::make_unique<impl>()) {};

tcp_server_async::~tcp_server_async() = default;

void tcp_server_async::run(std::unique_ptr<endpoint_ipv4>& ep) noexcept
{
	// Создание очереди сообщений между master_thread и worker_thread
	pimpl_->queue_ = std::make_unique<threadsafe_queue<std::unique_ptr<connection>>>();

	// Запуск master_thread
	pimpl_->master_thread_ = std::make_unique<master_thread>();
	pimpl_->master_thread_creator_ = std::make_unique<raii_thread>(std::bind(&master_thread::run,
	  std::ref(pimpl_->master_thread_), std::placeholders::_1, std::ref(pimpl_->queue_), std::ref(ep)));

	// Запуск worker_thread
	pimpl_->worker_thread_ = std::make_unique<worker_thread>();
	pimpl_->worker_thread_creator_ = std::make_unique<raii_thread>(std::bind(&worker_thread::run,
	std::ref(pimpl_->worker_thread_), std::placeholders::_1, std::ref(pimpl_->queue_)));

	//
	stop_master_threads_->connect(std::bind(&raii_thread::cancel, std::ref(pimpl_->master_thread_creator_)));
	stop_worker_threads_->connect(std::bind(&raii_thread::cancel, std::ref(pimpl_->worker_thread_creator_)));
}

void tcp_server_async::stop() noexcept
{
	stop_worker_threads_->emit();
	stop_master_threads_->emit();
}
