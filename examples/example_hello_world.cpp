#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

int main()
{
    auto io = asyncgi::IO{};
    auto router = asyncgi::Router{io};
    router.route("/", http::RequestMethod::Get)
            .process(
                    [](const asyncgi::Request&)
                    {
                        return http::Response{"Hello world"};
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