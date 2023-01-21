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
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        response.send("Welcome, admin!");
    }
};

struct ModerationPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        response.send("Welcome, moderator!");
    }
};

template<>
struct asyncgi::config::RouteMatcher<Access, RouteContext> {
    bool operator()(Access value, const asyncgi::Request&, asyncgi::Response&, RouteContext& context) const
    {
        return value == context.access;
    }
};


int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter<RouteContext>();
    router.route(asyncgi::rx{".*"}).process<Authorizer>();
    router.route("/admin", http::RequestMethod::Get, Access::Authorized).process<AdminPage>();
    // Internally RequestMethod parameter is implemented using RouteMatcher as well.
    // As you can see below, the order of provided RouteMatcher parameters isn't important.
    router.route("/moderation", Access::Authorized, http::RequestMethod::Get).process<ModerationPage>();
    router.route(asyncgi::rx{".*"}, Access::Forbidden)
            .set(http::ResponseStatus::_401_Unauthorized, "You are not authorized to view this page.");
    router.route().set(http::ResponseStatus::_404_Not_Found, "Page not found.");

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}