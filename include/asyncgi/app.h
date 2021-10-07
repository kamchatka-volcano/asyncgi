#pragma once
#include "iruntime.h"
#include "errors.h"
#include <string>
#include <memory>

namespace asyncgi{
class RequestProcessor;
class Server;
class Timer;
class App;

App makeApp(std::size_t workerThreadCount = 1);

class App
{
public:
    Server makeServer(RequestProcessor&, ErrorHandlerFunc errorHandler = ErrorHandlerFunc{});
    Timer makeTimer();
    void exec();

private:
    std::unique_ptr<IRuntime> runtime_;    

private:
    App(std::unique_ptr<IRuntime>);
    friend class AppFactory;
};

class AppFactory{
public:
    template<typename TRuntime, typename...TArgs>
    App createApp(TArgs&&... args)
    {
        static_assert (std::is_base_of_v<IRuntime, TRuntime>, "TRuntime must be a subclass of IRuntime");
        auto runtime = std::make_unique<TRuntime>(std::forward<TArgs>(args)...);
        return App{std::move(runtime)};
    }
};

}

