#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

struct Greeter{
    Greeter(const int& secondsCounter)
        : secondsCounter_{&secondsCounter}
    {
    }

    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        response.send("Hello world\n(alive for " + std::to_string(*secondsCounter_) + " seconds)");
    }

private:
    const int* secondsCounter_;
};

int main()
{
    auto io = asyncgi::IO{};
    int secondsCounter = 0;

    auto timer = asyncgi::Timer{io};
    timer.startPeriodic(
            std::chrono::seconds(1),
            [&secondsCounter]()
            {
                ++secondsCounter;
            });

    auto router = asyncgi::Router{io};
    router.route("/").process<Greeter>(secondsCounter);
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