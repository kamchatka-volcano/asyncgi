#include <asyncgi/app.h>
#include "server.h"
#include "timer.h"
#include "client.h"
#include "runtime.h"
#include "multithreadedruntime.h"
#include "connectionfactory.h"

namespace asyncgi::detail{

class App : public IApp
{
public:
    explicit App(std::unique_ptr<detail::IRuntime>);
    std::unique_ptr<IServer> makeServer(IRequestProcessor&) const override;
    std::unique_ptr<IServer> makeServer(IRequestProcessor&, ErrorHandlerFunc errorHandler) const override;
    std::unique_ptr<ITimer> makeTimer() const override;
    std::unique_ptr<IClient> makeClient(ErrorHandlerFunc errorHandler) const override;
    void exec() override;

private:
    std::unique_ptr<detail::IRuntime> runtime_;
};

App::App(std::unique_ptr<detail::IRuntime> runtime)
    : runtime_(std::move(runtime))
{
}

std::unique_ptr<IServer> App::makeServer(detail::IRequestProcessor& requestProcessor, ErrorHandlerFunc errorHandler) const
{
    auto connectionFactory = std::make_unique<detail::ConnectionFactory>(requestProcessor, *runtime_, errorHandler);
    return std::make_unique<detail::Server>(runtime_->io(), std::move(connectionFactory), errorHandler);
}

std::unique_ptr<IServer> App::makeServer(detail::IRequestProcessor& requestProcessor) const
{
    return makeServer(requestProcessor, {});
}


std::unique_ptr<ITimer> App::makeTimer() const
{
    return std::make_unique<Timer>(runtime_->io());
}

std::unique_ptr<IClient> App::makeClient(ErrorHandlerFunc errorHandler) const
{
    return std::make_unique<Client>(runtime_->io(), errorHandler);
}


void App::exec()
{
    runtime_->run();
}
}

namespace asyncgi {
std::unique_ptr<IApp> makeApp(std::size_t workerThreadCount)
{
    if (workerThreadCount <= 1)
        return std::make_unique<detail::App>(std::make_unique<detail::Runtime>());
    else
        return std::make_unique<detail::App>(std::make_unique<detail::MultithreadedRuntime>(workerThreadCount));
}
}

