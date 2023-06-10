#include <asyncgi/asyncgi.h>
#include <mutex>

namespace http = asyncgi::http;
using namespace std::string_literals;

enum class AccessRole {
    Admin,
    Guest
};

struct RouteContext {
    AccessRole role = AccessRole::Guest;
};

struct AdminAuthorizer {
    void operator()(const asyncgi::Request& request, asyncgi::Response&, RouteContext& context)
    {
        if (request.cookie("admin_id") == "ADMIN_SECRET")
            context.role = AccessRole::Admin;
    }
};

struct LoginPage {
    void operator()(const asyncgi::Request&, asyncgi::Response& response, RouteContext&)
    {
        response.send(R"(
                <html>
                <form method="post" enctype="multipart/form-data">
                <label for="msg">Login:</label>
                <input id="login" name="login" value="">
                <label for="msg">Password:</label>
                <input id="passwd" name="passwd" value="">
                <input value="Submit" data-popup="true" type="submit">
                </form></html>)");
    }
};

struct LoginPageAuthorize {
    void operator()(const asyncgi::Request& request, asyncgi::Response& response, RouteContext&)
    {
        if (request.formField("login") == "admin" && request.formField("passwd") == "12345")
            response.redirect(
                    "/",
                    asyncgi::http::RedirectType::Found,
                    {asyncgi::http::Cookie("admin_id", "ADMIN_SECRET")});
        else
            response.redirect("/login");
    }
};

template<>
struct asyncgi::config::RouteMatcher<AccessRole, RouteContext> {
    bool operator()(AccessRole value, const asyncgi::Request&, asyncgi::Response&, RouteContext& context) const
    {
        return value == context.role;
    }
};

int main()
{
    auto app = asyncgi::makeApp(4);
    auto router = asyncgi::makeRouter<RouteContext>();
    router.route(asyncgi::rx{".*"}).process<AdminAuthorizer>();
    router.route("/").process(
            [](const asyncgi::Request&, asyncgi::Response& response, RouteContext& context)
            {
                if (context.role == AccessRole::Admin)
                    response.send("<p>Hello admin</p>");
                else
                    response.send(R"(<p>Hello guest</p><p><a href="/login">login</a>)");
            });

    router.route("/login", http::RequestMethod::Get, AccessRole::Guest).process<LoginPage>();
    router.route("/login", http::RequestMethod::Post, AccessRole::Guest).process<LoginPageAuthorize>();
    router.route("/login", http::RequestMethod::Get, AccessRole::Admin).set("/", http::RedirectType::Found);
    router.route("/login", http::RequestMethod::Post, AccessRole::Admin).set("/", http::RedirectType::Found);
    router.route().set(http::ResponseStatus::_404_Not_Found, "Page not found");

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}