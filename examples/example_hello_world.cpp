#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::Get).set("Hello world");
    router.route().set(http::ResponseStatus::_404_Not_Found);

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}