#include <asyncgi/asyncgi.h>

using namespace asyncgi;

enum class Access{
    Authorized,
    Forbidden
};

struct RouteContext{
    Access access = Access::Forbidden;
};

struct Authorizer{
    void operator()(const asyncgi::Request& request, asyncgi::Response<RouteContext>& response)
    {
        if(request.cookie("admin_id") == "ADMIN_SECRET")
            response.context().access = Access::Authorized;
        else
            response.context().access = Access::Forbidden;
    }
};

struct AdminPage{
    void operator()(const asyncgi::Request&, asyncgi::Response<RouteContext>& response)
    {
        if (response.context().access == Access::Authorized)
            response.send("Welcome, admin!");
        else
            response.send(http::ResponseStatus::Code_401_Unauthorized, "You are not authorized to view this page.");
    }
};

struct ModerationPage{
    void operator()(const asyncgi::Request&, asyncgi::Response<RouteContext>& response)
    {
        if (response.context().access == Access::Authorized)
            response.send("Welcome, moderator!");
        else
            response.send(http::ResponseStatus::Code_401_Unauthorized, "You are not authorized to view this page.");
    }
};


int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter<RouteContext>();
    router.route(asyncgi::rx{".*"}).process<Authorizer>();
    router.route("/admin", http::RequestMethod::GET).process<AdminPage>();
    router.route("/moderation", http::RequestMethod::GET).process<ModerationPage>();
    router.route().set(http::ResponseStatus::Code_404_Not_Found, "Page not found.");

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}