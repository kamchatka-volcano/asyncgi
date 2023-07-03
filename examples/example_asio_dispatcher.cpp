#include <asyncgi/asyncgi.h>
#ifdef ASYNCGI_USE_BOOST_ASIO
namespace asio = boost::asio;
#include <boost/asio/steady_timer.hpp>
#else
#include <asio/steady_timer.hpp>
#endif
#include <iostream>

int main()
{
    auto io = asyncgi::IO{};
    auto disp = asyncgi::AsioDispatcher{io};
    disp.postTask(
            [&io](const asyncgi::TaskContext& ctx) mutable
            {
                auto timer = std::make_shared<asio::steady_timer>(ctx.io());
                timer->expires_after(std::chrono::seconds{3});
                timer->async_wait(
                        [timer, ctx, &io](auto&) mutable
                        {
                            std::cout << "Hello world" << std::endl;
                            io.stop();
                        });
            });
    io.run();
    return 0;
}