#include <asyncgi/asyncgi.h>
#include <asyncgi/http_client.h>

struct RequestPage{
    void operator()(const http::Request&, asyncgi::Responder& responder)
    {
        auto client = asyncgi::HttpClient{responder};
        client.makeRequest(
                http::Request{http::RequestMethod::Get, "https://api.ipify.org"},
                [responder](std::optional<http::Response> reqResponse) mutable
                {
                    if (reqResponse.has_value()) {
                        responder.send(std::string{reqResponse->body()});
                    }
                    else
                        responder.send("No response");
                });
    }
};

int main()
{
    auto io = asyncgi::IO{};
    auto router = asyncgi::Router{io};
    router.route("/", http::RequestMethod::Get).process<RequestPage>();
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = asyncgi::Server{io, router};
#ifndef _WIN32
    server.listen("/tmp/fcgi.sock");
#else
    server.listen("127.0.0.1", 9088);
#endif
    io.run();
    return 0;
}