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
    void operator()(const asyncgi::Request& request, asyncgi::Response&, RouteContext& context)
    {
        if (request.cookie("admin_id") == "ADMIN_SECRET")
            context.access = Access::Authorized;
        else
            context.access = Access::Forbidden;
    }
};

struct AdminPage{
    void operator()(
            const std::string& name,
            const asyncgi::Request&,
            asyncgi::Response& response,
            RouteContext& context)
    {
        if (context.access == Access::Authorized)
            response.send("Welcome, admin " + name + "!");
        else
            response.send(http::ResponseStatus::_401_Unauthorized, "You are not authorized to view this page.");
    }
};

struct ModerationPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response, RouteContext& context) const
    {
        if (context.access == Access::Authorized)
            response.send("Welcome, moderator!");
        else
            response.send(http::ResponseStatus::_401_Unauthorized, "You are not authorized to view this page.");
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter<RouteContext>();
    router.route(asyncgi::rx{".*"}).process<Authorizer>();
    router.route(asyncgi::rx{"/admin/(.+)"}, http::RequestMethod::Get).process<AdminPage>();
    router.route("/moderation", http::RequestMethod::Get).process<ModerationPage>();
    //router.route().set(http::ResponseStatus::Code_404_Not_Found, "Page not found.");
    router.route().process(
            [](const asyncgi::Request&, asyncgi::Response& response, RouteContext&)
            {
                response.send(http::ResponseStatus::_404_Not_Found, "Page not found.");
            });

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}