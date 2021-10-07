#include <asyncgi/app.h>
#include <asyncgi/server.h>
#include <asyncgi/timer.h>
#include "runtime.h"
#include "multithreadedruntime.h"
#include "connectionfactory.h"

namespace asyncgi{

App::App(std::unique_ptr<IRuntime> runtime)
    : runtime_(std::move(runtime))
{
}

App makeApp(std::size_t workerThreadCount)
{
    if (workerThreadCount <= 1)
        return AppFactory{}.createApp<Runtime>();
    else
        return AppFactory{}.createApp<MultithreadedRuntime>(workerThreadCount);
}

Server App::makeServer(RequestProcessor& requestProcessor, ErrorHandlerFunc errorHandler)
{
    auto connectionFactory = std::make_unique<ConnectionFactory>(requestProcessor, *runtime_, errorHandler);
    return Server{runtime_->io(), std::move(connectionFactory), errorHandler};
}

Timer App::makeTimer()
{
    return Timer{runtime_->io()};
}

void App::exec()
{
    runtime_->run();
}

}
