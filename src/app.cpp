#include "asiodispatcher.h"
#include "client.h"
#include "connectionfactory.h"
#include "connectionlistenerfactory.h"
#include "multithreadedruntime.h"
#include "runtime.h"
#include "server.h"
#include "timer.h"
#include <asyncgi/asyncgi.h>
#include <asyncgi/requestprocessor.h>

namespace asyncgi::detail {

class App : public IApp {
public:
    explicit App(std::unique_ptr<detail::IRuntime>);
    std::unique_ptr<IServer> makeServer(RequestProcessor) const override;
    std::unique_ptr<IServer> makeServer(RequestProcessor, ErrorHandlerFunc errorHandler) const override;
    std::unique_ptr<ITimer> makeTimer() const override;
    std::unique_ptr<IClient> makeClient() const override;
    std::unique_ptr<IClient> makeClient(ErrorHandlerFunc errorHandler) const override;
    std::unique_ptr<IAsioDispatcher> makeAsioDispatcher() const override;
    void exec() override;
    void exit() override;

private:
    std::unique_ptr<detail::IRuntime> runtime_;
};

App::App(std::unique_ptr<detail::IRuntime> runtime)
    : runtime_{std::move(runtime)}
{
}

std::unique_ptr<IServer> App::makeServer(RequestProcessor requestProcessor, ErrorHandlerFunc errorHandler) const
{
    auto connectionFactory =
            std::make_unique<detail::ConnectionFactory>(std::move(requestProcessor), *runtime_, errorHandler);
    auto connectionListenerFactory = std::make_unique<detail::ConnectionListenerFactory>(
            runtime_->io(),
            std::move(connectionFactory),
            errorHandler);
    return std::make_unique<detail::Server>(std::move(connectionListenerFactory));
}

std::unique_ptr<IServer> App::makeServer(RequestProcessor requestProcessor) const
{
    return makeServer(std::move(requestProcessor), {});
}

std::unique_ptr<ITimer> App::makeTimer() const
{
    return std::make_unique<Timer>(runtime_->io());
}

std::unique_ptr<IClient> App::makeClient() const
{
    return std::make_unique<Client>(runtime_->io(), ErrorHandlerFunc{});
}

std::unique_ptr<IClient> App::makeClient(ErrorHandlerFunc errorHandler) const
{
    return std::make_unique<Client>(runtime_->io(), errorHandler);
}

void App::exec()
{
    runtime_->run();
}

void App::exit()
{
    runtime_->stop();
}

std::unique_ptr<IAsioDispatcher> App::makeAsioDispatcher() const
{
    return std::make_unique<AsioDispatcher>(runtime_->io());
}

} // namespace asyncgi::detail

namespace asyncgi {
std::unique_ptr<IApp> makeApp(std::size_t workerThreadCount)
{
    if (workerThreadCount <= 1)
        return std::make_unique<detail::App>(std::make_unique<detail::Runtime>());
    else
        return std::make_unique<detail::App>(std::make_unique<detail::MultithreadedRuntime>(workerThreadCount));
}
} // namespace asyncgi
