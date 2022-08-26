#include <asyncgi/asyncgi.h>
#include <asio/steady_timer.hpp>
#include <iostream>

int main()
{
    auto app = asyncgi::makeApp();
    auto disp = app->makeAsioDispatcher();
    disp->postTask([](const asyncgi::TaskContext& ctx) mutable
                {
                    auto timer = std::make_shared<asio::steady_timer>(ctx.io());
                    timer->expires_after(std::chrono::seconds{3});
                    timer->async_wait([timer, ctx](auto&) mutable{
                        std::cout << "Hello world" << std::endl;
                    });
                });
    app->exec();
    return 0;
}