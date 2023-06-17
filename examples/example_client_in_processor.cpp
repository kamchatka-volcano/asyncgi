#include <asyncgi/asyncgi.h>

using namespace asyncgi;

struct RequestPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        // making request to FastCgi application listening on /tmp/fcgi.sock and showing the received response
        response.makeRequest(
#ifndef _WIN32
                "/tmp/fcgi.sock",
#else
                "127.0.0.1", 9088,
#endif
                http::Request{http::RequestMethod::Get, "/"},
                [response](const std::optional<http::ResponseView>& reqResponse) mutable
                {
                    if (reqResponse)
                        response.send(std::string{reqResponse->body()});
                    else
                        response.send("No response");
                });
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::Get).process<RequestPage>();
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = app->makeServer(router);
#ifndef _WIN32
    server->listen("/tmp/fcgi_client.sock");
#else
    server->listen("127.0.0.1", 9089);
#endif
    app->exec();
    return 0;
}