#include <asyncgi/asyncgi.h>

fastcgi::Response guestBookPage(const fastcgi::Request& request)
{
    if (request.param("REQUEST_URI") == "/")
        return {"Status: 200 OK \r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<h1>Guest book</h1>\n"
                "<p>No messages</p>\n"};
    return {"Status: 404 Not found\r\n\r\n"};
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