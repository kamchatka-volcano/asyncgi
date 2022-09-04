#include <asyncgi/asyncgi.h>

enum class Access{
    Authorized,
    Forbidden
};

struct RouteContext{
    Access access = Access::Forbidden;
};

struct Authorizer : asyncgi::RequestProcessor<RouteContext>{
    void process(const asyncgi::Request& request, asyncgi::Response<RouteContext>& response) override
    {
        if(request.cookie("admin_id") == "ADMIN_SECRET")
            response.context().access = Access::Authorized;
        else
            response.context().access = Access::Forbidden;
    }
};

struct AdminPage : asyncgi::RequestProcessor<RouteContext>{
    void process(const asyncgi::Request&, asyncgi::Response<RouteContext>& response) override
    {
       response.send("Welcome, admin!");
    }
};

struct ModerationPage : asyncgi::RequestProcessor<RouteContext>{
    void process(const asyncgi::Request&, asyncgi::Response<RouteContext>& response) override
    {
        response.send("Welcome, moderator!");
    }
};

template<>
struct asyncgi::config::RouteSpecification<Access, asyncgi::Request, asyncgi::Response<RouteContext>> {
    bool operator()(Access value, const asyncgi::Request&, asyncgi::Response<RouteContext>& response) const
    {
        return value == response.context().access;
    }
};


int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter<RouteContext>();
    router.route(asyncgi::rx{".*"}).process<Authorizer>();
    router.route("/admin", http::RequestMethod::GET, Access::Authorized).process<AdminPage>();
    //RequestMethod parameter is implemented using RouteSpecification as well.
    //As you can see below, the order of provided RouteSpecification parameters isn't important.
    router.route("/moderation", Access::Authorized, http::RequestMethod::GET).process<ModerationPage>();
    router.route(asyncgi::rx{".*"}, Access::Forbidden).set(http::ResponseStatus::Code_401_Unauthorized, "You are not authorized to view this page.");
    router.route().set(http::ResponseStatus::Code_404_Not_Found, "Page not found.");

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}