#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

class GuestBookPage {
public:
    void operator()(const asyncgi::Request& request, asyncgi::Response& response)
    {
        if (request.path() == "/")
            response.send(R"(
                <h1>Guest book</h1>
                <p>No messages</p>
            )");
        else
            response.send(http::ResponseStatus::_404_Not_Found);
    }
};

void showGuestBookPage(const asyncgi::Request& request, asyncgi::Response& response)
{
    if (request.path() == "/")
        response.send(R"(
                <h1>Guest book</h1>
                <p>No messages</p>
            )");
    else
        response.send(http::ResponseStatus::_404_Not_Found);
}

int main()
{
    auto app = asyncgi::makeApp();
    //auto guestBookPage = GuestBookPage{};
    auto server = app->makeServer(showGuestBookPage);
    //Listen for FastCGI connections on UNIX domain socket
    server->listen("/tmp/fcgi.sock");
    //or over TCP
    //server->listen("127.0.0.1", 9000);
    app->exec();
    return 0;
}