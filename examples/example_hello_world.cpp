#include <asyncgi/asyncgi.h>

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::GET).set("Hello world");
    router.route().set(http::ResponseStatus::Code_404_Not_Found);

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}