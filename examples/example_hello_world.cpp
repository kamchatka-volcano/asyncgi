#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::Get).set("Hello world");
    router.route().set(http::ResponseStatus::_404_Not_Found);

    auto server = app->makeServer(router);
#ifndef _WIN32
    server->listen("/tmp/fcgi.sock");
#else
    server->listen("127.0.0.1", 9088);
#endif
    app->exec();
    return 0;
}