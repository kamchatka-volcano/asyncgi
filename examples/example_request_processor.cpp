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

int main()
{
    auto app = asyncgi::makeApp();
    auto guestBookPage = GuestBookPage{};
    auto server = app->makeServer(guestBookPage);
#ifndef _WIN32
    server->listen("/tmp/fcgi.sock");
#else
    server->listen("127.0.0.1", 9088);
#endif
    app->exec();
    return 0;
}