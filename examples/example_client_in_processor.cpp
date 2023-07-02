#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

struct RequestPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        // making request to FastCgi application listening on /tmp/fcgi.sock and showing the received response
        auto client = asyncgi::Client{response};
        client.makeRequest(
#ifndef _WIN32
                "/tmp/fcgi.sock",
#else
                "127.0.0.1",
                9088,
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
    auto io = asyncgi::IO{};
    auto router = asyncgi::Router{};
    router.route("/", http::RequestMethod::Get).process<RequestPage>();
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = asyncgi::Server{io, router};
#ifndef _WIN32
    server.listen("/tmp/fcgi_client.sock");
#else
    server.listen("127.0.0.1", 9089);
#endif
    io.run();
    return 0;
}