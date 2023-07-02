#include <asyncgi/asyncgi.h>
#include <thread>

using namespace asyncgi;

struct DelayedPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        auto timer = asyncgi::Timer{response};
        timer.waitFuture(
                std::async(
                        std::launch::async,
                        []
                        {
                            std::this_thread::sleep_for(std::chrono::seconds(3));
                            return "world";
                        }),
                [response](const std::string& result) mutable
                {
                    response.send(http::Response{"Hello " + result});
                });
    }
};

int main()
{
    auto io = asyncgi::IO{};
    auto router = asyncgi::Router{};
    auto delayedPage = DelayedPage{};
    router.route("/", http::RequestMethod::Get).process(delayedPage);
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = asyncgi::Server{io, router};
#ifndef _WIN32
    server.listen("/tmp/fcgi.sock");
#else
    server.listen("127.0.0.1", 9088);
#endif
    io.run();
    return 0;
}