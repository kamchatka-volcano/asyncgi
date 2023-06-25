#include <asyncgi/asyncgi.h>
#include <iostream>

using namespace asyncgi;

int main()
{
    auto io = asyncgi::IO{};
    auto client = asyncgi::Client{io};
    client.makeRequest(
#ifndef _WIN32
            "/tmp/fcgi.sock",
#else
            "127.0.0.1",
            9088,
#endif
            http::Request{http::RequestMethod::Get, "/"},
            [&io](const std::optional<http::ResponseView>& response)
            {
                if (response)
                    std::cout << response->body() << std::endl;
                else
                    std::cout << "No response" << std::endl;
                io.stop();
            });
    io.run();
    return 0;
}