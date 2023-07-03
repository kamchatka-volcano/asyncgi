#pragma once
#ifdef ASYNCGI_USE_BOOST_ASIO
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#else
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/signal_set.hpp>
#endif
#include <asyncgi/detail/asio_namespace.h>
#include <asyncgi/errors.h>
#include <memory>
#include <thread>
#include <vector>

namespace asyncgi::detail {

using asio_work_guard = asio::executor_work_guard<asio::io_context::executor_type>;

class IOService {
public:
    explicit IOService(int threadCount);
    asio::io_context& io() const;
    asio::io_context& nextIO();
    void run();
    void stop();

private:
    void handleStopSignals();

private:
    std::vector<std::unique_ptr<asio::io_context>> ioPool_;
    std::vector<std::unique_ptr<asio_work_guard>> ioGuardPool_;
    asio::signal_set signals_;
    std::vector<std::thread> threadPool_;
    std::atomic<std::size_t> ioIndex_ = 0;
};

} // namespace asyncgi::detail
