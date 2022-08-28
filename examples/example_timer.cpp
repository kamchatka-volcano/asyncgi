#include <asyncgi/asyncgi.h>

struct Greeter : asyncgi::RequestProcessor<>{
    Greeter(const int& secondsCounter)
        : secondsCounter_{secondsCounter}
    {
    }

    void process(const asyncgi::Request&, asyncgi::Response<>& response) override
    {
        response.send("Hello world\n(alive for " + std::to_string(secondsCounter_) + " seconds)");
    }

private:
    const int& secondsCounter_;
};

int main()
{
    auto app = asyncgi::makeApp();
    int secondsCounter = 0;

    auto timer = app->makeTimer();
    timer->start(std::chrono::seconds(1), [&secondsCounter](){
        ++secondsCounter;
    }, asyncgi::TimerMode::Repeatedly);

    auto router = asyncgi::makeRouter();
    router.route("/").process<Greeter>(secondsCounter);
    router.route().set(http::ResponseStatus::Code_404_Not_Found);

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}