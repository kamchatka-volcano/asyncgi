#include <asyncgi/asyncgi.h>

using namespace asyncgi;

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
    auto app = asyncgi::makeApp();
    int secondsCounter = 0;

    auto timer = app->makeTimer();
    timer->startPeriodic(
            std::chrono::seconds(1),
            [&secondsCounter]()
            {
                ++secondsCounter;
            });

    auto router = asyncgi::makeRouter();
    router.route("/").process<Greeter>(secondsCounter);
    router.route().set(http::ResponseStatus::_404_Not_Found);

    auto server = app->makeServer(router);
#ifndef _WIN32
    server->listen("/tmp/fcgi.sock");
#else
    server->listen("127.0.0.1", 9088);
#endif
    app->exec();
    return 0;
}