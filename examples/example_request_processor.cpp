#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

http::Response guestBookPage(const asyncgi::Request& request)
{
    if (request.path() == "/")
        return {R"(
                <h1>Guest book</h1>
                <p>No messages</p>
            )"};

    return http::ResponseStatus::_404_Not_Found;
}

int main()
{
    auto io = asyncgi::IO{};
    auto server = asyncgi::Server{io, guestBookPage};
#ifndef _WIN32
    server.listen("/tmp/fcgi.sock");
#else
    server.listen("127.0.0.1", 9088);
#endif
    io.run();
    return 0;
}