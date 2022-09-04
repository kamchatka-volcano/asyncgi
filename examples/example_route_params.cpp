#include <asyncgi/asyncgi.h>
#include <tuple>

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
            response.context().access = Access::Authorized;
    }
};

struct AdminPage : asyncgi::RequestProcessor<RouteContext, std::string>{
    void process(const std::string& name, const asyncgi::Request&, asyncgi::Response<RouteContext>& response) override
    {
        if (response.context().access == Access::Authorized)
            response.send("Welcome, admin " + name + "!");
        else
            response.send(http::ResponseStatus::Code_401_Unauthorized, "You are not authorized to view this page.");
    }
};

struct ModerationPage : asyncgi::RequestProcessor<RouteContext>{
    void process(const asyncgi::Request&, asyncgi::Response<RouteContext>& response) override
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
    router.route(asyncgi::rx{"/admin/(.+)"}, http::RequestMethod::GET).process<AdminPage>();
    router.route("/moderation", http::RequestMethod::GET).process<ModerationPage>();
    //router.route().set(http::ResponseStatus::Code_404_Not_Found, "Page not found.");
    router.route().process([](const asyncgi::Request&, asyncgi::Response<RouteContext>& response){
        response.send(http::ResponseStatus::Code_404_Not_Found, "Page not found.");});

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}