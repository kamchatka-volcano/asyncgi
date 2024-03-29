#include <asyncgi/asyncgi.h>
#include <mutex>
#include <optional>
#include <regex>

namespace http = asyncgi::http;
using namespace std::string_literals;

enum class AccessRole {
    Admin,
    Guest
};

struct RouteContext {
    AccessRole role = AccessRole::Guest;
};

template<>
struct asyncgi::config::RouteMatcher<AccessRole, RouteContext> {
    bool operator()(AccessRole value, const asyncgi::Request&, const RouteContext& context) const
    {
        return value == context.role;
    }
};

std::optional<http::Response> authorizeAdmin(const asyncgi::Request& request, RouteContext& context)
{
    if (request.cookie("admin_id") == "ADMIN_SECRET")
        context.role = AccessRole::Admin;

    return std::nullopt;
}

http::Response showLoginPage(const asyncgi::Request&)
{
    return {R"(
            <head><link rel="stylesheet" href="https://cdn.simplecss.org/simple.min.css"></head>
            <form method="post" enctype="multipart/form-data">
                <label for="msg">Login:</label>
                <input id="login" name="login" value="">
                <label for="msg">Password:</label>
                <input id="passwd" name="passwd" value="">
                <input value="Submit" data-popup="true" type="submit">
            </form>)"};
}

http::Response loginAdmin(const asyncgi::Request& request)
{
    if (request.formField("login") == "admin" && request.formField("passwd") == "12345")
        return {http::Redirect{"/"}, {http::Cookie("admin_id", "ADMIN_SECRET")}};
    else
        return http::Redirect{"/login"};
}

http::Response logoutAdmin(const asyncgi::Request&)
{
    return {http::Redirect{"/"}, {http::Cookie("admin_id", "")}};
}

struct GuestBookMessage {
    std::string name;
    std::string text;
};

class GuestBookState {
public:
    std::vector<GuestBookMessage> messages()
    {
        auto lock = std::scoped_lock{mutex_};
        return messages_;
    }

    void addMessage(std::string name, std::string msg)
    {
        name = std::regex_replace(name, std::regex{"<"}, "&lt;");
        name = std::regex_replace(name, std::regex{">"}, "&gt;");
        msg = std::regex_replace(msg, std::regex{"<"}, "&lt;");
        msg = std::regex_replace(msg, std::regex{">"}, "&gt;");
        auto lock = std::scoped_lock{mutex_};
        messages_.emplace_back(GuestBookMessage{name.empty() ? "Guest" : name, msg});
    }

    void removeMessage(int index)
    {
        auto lock = std::scoped_lock{mutex_};
        if (index < 0 || index >= static_cast<int>(messages_.size()))
            return;
        messages_.erase(std::next(messages_.begin(), index));
    }

private:
    std::vector<GuestBookMessage> messages_;
    std::mutex mutex_;
};

std::string makeMessagesDiv(const std::vector<GuestBookMessage>& messages, AccessRole role)
{
    if (messages.empty())
        return "<div>No messages</div>";

    auto messagesDiv = std::string{};
    for (auto i = 0; i < static_cast<int>(messages.size()); ++i) {
        messagesDiv += "<h4>" + messages.at(i).name + " says:</hr><pre>" + messages.at(i).text + "</pre>";
        if (role == AccessRole::Admin)
            messagesDiv += R"(<form action="/delete/)" + std::to_string(i) +
                    R"(" method="post"> <input value="Delete" type="submit"></form>)";
    }
    return messagesDiv;
}

std::string makeLinksDiv(AccessRole role)
{
    return (role == AccessRole::Admin ? R"(<a href="/logout">logout</a>&nbsp;)"s
                                      : R"(<a href="/login">login</a>&nbsp;)"s) +
            R"(<a href="https://github.com/kamchatka-volcano/asyncgi/blob/master/examples/example_guestbook.cpp">source</a></div>)"s;
}

auto showGuestBookPage(GuestBookState& state)
{
    return [&state](const asyncgi::Request& request, RouteContext& context) -> http::Response
    {
        auto page = R"(<head><link rel="stylesheet" href="https://cdn.simplecss.org/simple.min.css"></head>
                       <div style="display:flex; flex-direction: row; justify-content: flex-end">%LINKS%</div>
                       <div style="display:flex; flex-direction: column; height: calc(100vh - 5em);">
                           <h1>asyncgi guest book</h1>
                           <div style="flex: 1; min-height: 0; display:flex; flex-direction: column;">
                               <div style="flex:1; overflow-y: auto; row-gap: 60px">
                                    %MESSAGES%
                               </div>
                           </div>
                           <hr>
                           <div>
                                %ERROR_MSG%
                               <form style="display:flex; flex-direction: column; width:66%;" method="post" enctype="multipart/form-data">
                                   <label for="name">Name:</label>
                                   <input id="name" name="name" style="width:50%">
                                   <textarea style="min-height:5em; resize:none;" name="msg" id="msg" maxlength="4192" autocomplete="off"></textarea>
                                   <div style="display:flex; flex-direction: row; justify-content: flex-end">
                                      <input value="Submit" data-popup="true" type="submit" style="width:33%"></td>
                                   </div>
                               </form>
                           </div>
                       </div>)"s;

        page = std::regex_replace(page, std::regex{"%MESSAGES%"}, makeMessagesDiv(state.messages(), context.role));
        page = std::regex_replace(page, std::regex{"%LINKS%"}, makeLinksDiv(context.role));
        if (request.hasQuery("error")) {
            if (request.query("error") == "urls_in_msg")
                page = std::regex_replace(page, std::regex{"%ERROR_MSG%"}, "<mark>Messages can't contain urls</mark>");
            if (request.query("error") == "empty_msg")
                page = std::regex_replace(page, std::regex{"%ERROR_MSG%"}, "<mark>Messages can't be empty</mark>");
        }
        else
            page = std::regex_replace(page, std::regex{"%ERROR_MSG%"}, "");

        return page;
    };
}

auto addMessage(GuestBookState& state)
{
    return [&state](const asyncgi::Request& request) -> http::Response
    {
        if (std::all_of(
                    request.formField("msg").begin(),
                    request.formField("msg").end(),
                    [](char ch)
                    {
                        return std::isspace(static_cast<unsigned char>(ch));
                    }))
            return http::Redirect{"/?error=empty_msg"};
        else if (
                request.formField("msg").find("http://") != std::string_view::npos ||
                request.formField("msg").find("https://") != std::string_view::npos)
            return http::Redirect{"/?error=urls_in_msg"};
        else {
            state.addMessage(std::string{request.formField("name")}, std::string{request.formField("msg")});
            return http::Redirect{"/"};
        }
    };
}

auto removeMessage(GuestBookState& state)
{
    return [&state](int index, const asyncgi::Request&) -> http::Response
    {
        state.removeMessage(index);
        return http::Redirect{"/"};
    };
}

int main()
{
    auto io = asyncgi::IO{4};
    auto state = GuestBookState{};
    auto router = asyncgi::Router<RouteContext>{io};
    router.route(asyncgi::rx{".*"}).process(authorizeAdmin);
    router.route("/", http::RequestMethod::Get).process(showGuestBookPage(state));
    router.route("/", http::RequestMethod::Post).process(addMessage(state));
    router.route(asyncgi::rx{"/delete/(.+)"}, http::RequestMethod::Post, AccessRole::Admin)
            .process(removeMessage(state));
    router.route(asyncgi::rx{"/delete/(.+)"}, http::RequestMethod::Post, AccessRole::Guest)
            .set(http::ResponseStatus::_401_Unauthorized);
    router.route("/login", http::RequestMethod::Get, AccessRole::Guest).process(showLoginPage);
    router.route("/login", http::RequestMethod::Post, AccessRole::Guest).process(loginAdmin);
    router.route("/logout").process(logoutAdmin);
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