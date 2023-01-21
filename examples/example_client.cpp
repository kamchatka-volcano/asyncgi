#include <asyncgi/asyncgi.h>
#include <iostream>

using namespace asyncgi;

int main()
{
    auto app = asyncgi::makeApp();
    auto client = app->makeClient();
    client->makeRequest("/tmp/fcgi.sock", http::Request{http::RequestMethod::Get, "/"},
            [](const std::optional<http::ResponseView>& response){
                if (response)
                    std::cout << response->body() << std::endl;
                else
                    std::cout << "No response" << std::endl;
            }
    );
    app->exec();
    return 0;
}