#include <asyncgi/asyncgi.h>

using namespace std::string_literals;
struct Greeter : asyncgi::RequestProcessor<>{
    void process(
            const asyncgi::Request& request,
            asyncgi::Response<>& response) override
    {
        if (request.path() == "/")
            response.send("Hello world"s);
        else
            response.send(http::ResponseStatus::Code_404_Not_Found);
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto greeter = Greeter{};
    auto server = app->makeServer(greeter);
    //Listen for FastCGI connections on UNIX domain socket
    server->listen("/tmp/fcgi.sock");
    //or over TCP
    //server->listen("127.0.0.1", 9000);
    app->exec();
    return 0;
}