#include <asyncgi/asyncgi.h>
#include <mutex>
#include <optional>

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
    std::optional<http::Response> operator()(const asyncgi::Request& request, RouteContext& context)
    {
        if (request.cookie("admin_id") == "ADMIN_SECRET")
            context.role = AccessRole::Admin;

        return std::nullopt;
    }
};

struct LoginPage {
    http::Response operator()(const asyncgi::Request&, RouteContext& context)
    {
        if (context.role == AccessRole::Guest)
            return {R"(
                    <html>
                    <form method="post" enctype="multipart/form-data">
                    <label for="msg">Login:</label>
                    <input id="login" name="login" value="">
                    <label for="msg">Password:</label>
                    <input id="passwd" name="passwd" value="">
                    <input value="Submit" data-popup="true" type="submit">
                    </form></html>)"};
        else //We are already logged in as the administrator
            return http::Redirect{"/"};
    }
};

struct LoginPageAuthorize {
    http::Response operator()(const asyncgi::Request& request, RouteContext& context)
    {
        if (context.role == AccessRole::Guest) {
            if (request.formField("login") == "admin" && request.formField("passwd") == "12345")
                return {http::Redirect{"/"}, {asyncgi::http::Cookie("admin_id", "ADMIN_SECRET")}};
            else
                return http::Redirect{"/login"};
        }
        else //We are already logged in as the administrator
            return http::Redirect{"/"};
    }
};

int main()
{
    auto io = asyncgi::IO{4}; //4 threads processing requests
    auto router = asyncgi::Router<RouteContext>{io};
    router.route(asyncgi::rx{".*"}).process<AdminAuthorizer>();
    router.route("/").process(
            [](const asyncgi::Request&, asyncgi::Responder& response, RouteContext& context)
            {
                if (context.role == AccessRole::Admin)
                    response.send("<p>Hello admin</p>");
                else
                    response.send(R"(<p>Hello guest</p><p><a href="/login">login</a>)");
            });

    router.route("/login", http::RequestMethod::Get).process<LoginPage>();
    router.route("/login", http::RequestMethod::Post).process<LoginPageAuthorize>();
    router.route().set(http::ResponseStatus::_404_Not_Found, "Page not found");

    auto server = asyncgi::Server{io, router};
#ifndef _WIN32
    server.listen("/tmp/fcgi.sock");
#else
    server.listen("127.0.0.1", 9088);
#endif
    io.run();
    return 0;
}