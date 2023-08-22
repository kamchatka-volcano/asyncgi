#include <asyncgi/asyncgi.h>
#include <iostream>
#include <string_view>

namespace http = asyncgi::http;

void errorHandler(asyncgi::ErrorEvent, std::string_view message)
{
    std::cerr << message << std::endl;
}

int main()
{
    auto io = asyncgi::IO{errorHandler};
    auto router = asyncgi::Router{io};
    router.route("/", http::RequestMethod::Get)
            .process(
                    [](const asyncgi::Request&, asyncgi::Response&)
                    {
                        throw std::runtime_error{"Can't send a response"};
                    });

    auto server = asyncgi::Server{io, router};
#ifndef _WIN32
    server.listen("/tmp/fcgi.sock");
#else
    server.listen("127.0.0.1", 9088);
#endif
    io.run();
    return 0;
}