#include <asyncgi/asyncgi.h>

using namespace asyncgi;

struct RequestPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        // making request to FastCgi application listening on 127.0.0.1:9000 and showing the received response
        response.makeRequest(
                "127.0.0.1",
                9000,
                http::Request{http::RequestMethod::GET, "/"},
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
    router.route("/", http::RequestMethod::GET).process<RequestPage>();
    router.route().set(http::ResponseStatus::Code_404_Not_Found);
    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}