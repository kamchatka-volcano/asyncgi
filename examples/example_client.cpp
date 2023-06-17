#include <asyncgi/asyncgi.h>
#include <iostream>

using namespace asyncgi;

int main()
{
    auto app = asyncgi::makeApp();
    auto client = app->makeClient();
    client->makeRequest(
#ifndef _WIN32
            "/tmp/fcgi.sock",
#else
            "127.0.0.1",
            9088,
#endif
            http::Request{http::RequestMethod::Get, "/"},
            [&app](const std::optional<http::ResponseView>& response)
            {
                if (response)
                    std::cout << response->body() << std::endl;
                else
                    std::cout << "No response" << std::endl;
                app->exit();
            });
    app->exec();
    return 0;
}