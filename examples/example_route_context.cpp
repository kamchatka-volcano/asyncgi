#include <asyncgi/asyncgi.h>

using namespace std::string_literals;

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
        if (response.context().access == Access::Authorized)
            response.send("Welcome, admin!"s);
        else
            response.send(http::Response{http::ResponseStatus::Code_401_Unauthorized, "You are not authorized to view this page."s});
    }
};

struct ModerationPage : asyncgi::RequestProcessor<RouteContext>{
    void process(const asyncgi::Request&, asyncgi::Response<RouteContext>& response) override
    {
        if (response.context().access == Access::Authorized)
            response.send("Welcome, moderator!"s);
        else
            response.send(http::Response{http::ResponseStatus::Code_401_Unauthorized, "You are not authorized to view this page."s});
    }
};


int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter<RouteContext>();
    router.route(std::regex{".*"}).process<Authorizer>();
    router.route("/admin", http::RequestMethod::GET).process<AdminPage>();
    router.route("/moderation", http::RequestMethod::GET).process<ModerationPage>();
    router.route().set(http::Response{http::ResponseStatus::Code_404_Not_Found, "Page not found."});

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}