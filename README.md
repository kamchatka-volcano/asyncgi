<p align="center">
  <img height="128" src="doc/logo.png"/>
</p>

**asyncgi** - is a C++17 asynchronous microframework for creating web applications interfacing with any HTTP server supporting [FastCGI](https://en.wikipedia.org/wiki/FastCGI) protocol. It aims to provide a modern way of using [CGI](https://en.wikipedia.org/wiki/Common_Gateway_Interface), with a custom performant FastCGI implementation in C++, multithreading support and a clean and simple API:
 
```c++
#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

int main()
{
    auto io = asyncgi::IO{};
    auto router = asyncgi::Router{};
    router.route("/", http::RequestMethod::Get).process(
          [](const asyncgi::Request&, asyncgi::Response& response)
          {
              response.send("Hello world");
          });

    auto server = asyncgi::Server{io, router};
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

## Table of Contents
*    [Usage](#usage)
     * [Connection](#connection)
     * [Request processor](#request-processor)
     * [Router](#router)
     * [Route parameters](#route-parameters)
     * [Route context](#route-context)
     * [Route matchers](#route-matchers)
     * [Complete guest book example](#complete-guest-book-example)
     * [Timer](#timer)
     * [Client](#client)
     * [Executing an asio task](#executing-an-asio-task)
*    [Development status](#development-status)     
*    [Installation](#installation)
* [Building examples](#building-examples)
* [Running functional tests](#running-functional-tests)
* [License](#license)

## Usage

### Connection
Web applications developed with `asyncgi` require to establish a FastCGI connection with a web server handling HTTP requests. Most popular servers provide this functionality, for example `NGINX` can be used with a following configuration:

```
server {
	listen 8088;
	server_name localhost;
	index /~;	
	location / {
		try_files $uri $uri/ @fcgi;
	}
	
	location @fcgi {	
		fastcgi_pass  unix:/tmp/fcgi.sock;		
		#or using a TCP socket
		#fastcgi_pass localhost:9000;
		include fastcgi_params;		
		fastcgi_keep_conn off;	    
	}
}

```
`asyncgi` supports both `UNIX domain` and `TCP` sockets for opening `FastCGI` connections.

### Request processor

In order to process requests, it's necessary to provide a function or function object that fulfills
the `RequestProcessor` requirement. This means that the function should be invocable with the
arguments `const asyncgi::Request&, asyncgi::Response&`, and have a `void` return type.

<details>
  <summary>Example</summary>

```c++
///examples/example_request_processor.cpp
///
#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

void guestBookPage(const asyncgi::Request& request, asyncgi::Response& response)
{
    if (request.path() == "/")
        response.send(R"(
                <h1>Guest book</h1>
                <p>No messages</p>
            )");
    else
        response.send(http::ResponseStatus::_404_Not_Found);
}

int main()
{
    auto io = asyncgi::IO{};
    auto server = asyncgi::Server{io, guestBookPage};
    //Listen for FastCGI connections on UNIX domain socket
    server.listen("/tmp/fcgi.sock");
    //or over TCP
    //server.listen("127.0.0.1", 9088);
    io.run();
    return 0;
}
```

</details>

### Router

Multiple request processors can be registered in the `asyncgi::Router` object, where they are matched to the paths
specified in requests. The `asyncgi::Router` itself satisfies the `RequestProcessor` requirement.

If multiple threads are required for request processing, the desired number of workers can be passed to
the `asyncgi::IO` object's constructor. In such cases, the user must ensure that any shared data in the request
processors is protected from concurrent read/write access.

<details>
  <summary>Example</summary>

```c++
///examples/example_router.cpp
///
#include <asyncgi/asyncgi.h>
#include <mutex>

namespace http = asyncgi::http;
using namespace std::string_literals;

class GuestBookState {
public:
    std::vector<std::string> messages()
    {
        auto lock = std::scoped_lock{mutex_};
        return messages_;
    }

    void addMessage(const std::string& msg)
    {
        auto lock = std::scoped_lock{mutex_};
        messages_.emplace_back(msg);
    }

private:
    std::vector<std::string> messages_;
    std::mutex mutex_;
};

class GuestBookPage {
public:
    GuestBookPage(GuestBookState& state)
        : state_(&state)
    {
    }

    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        auto messages = state_->messages();
        auto page = "<h1>Guest book</h1>"s;
        if (messages.empty())
            page += "<p>No messages</p>";
        else
            for (const auto& msg : messages)
                page += "<p>" + msg + "</p>";

        page += "<hr>";
        page += "<form method=\"post\" enctype=\"multipart/form-data\">"
                "<label for=\"msg\">Message:</label>"
                "<input id=\"msg\" name=\"msg\" value=\"\">"
                "<input value=\"Submit\" data-popup=\"true\" type=\"submit\">"
                "</form>";
        response.send(page);
    }

private:
    GuestBookState* state_;
};

class GuestBookAddMessage {
public:
    GuestBookAddMessage(GuestBookState& state)
        : state_(&state)
    {
    }

    void operator()(const asyncgi::Request& request, asyncgi::Response& response)
    {
        state_->addMessage(std::string{request.formField("msg")});
        response.redirect("/");
    }

private:
    GuestBookState* state_;
};

int main()
{
    auto io = asyncgi::IO{4}; //4 threads processing requests
    auto state = GuestBookState{};
    auto router = asyncgi::Router{};
    router.route("/", http::RequestMethod::Get).process<GuestBookPage>(state);
    router.route("/", http::RequestMethod::Post).process<GuestBookAddMessage>(state);
    router.route().set(http::Response{http::ResponseStatus::_404_Not_Found, "Page not found"});
    //Alternatively, it's possible to pass arguments for creation of http::Response object to the set() method.
    //router.route().set(http::ResponseStatus::Code_404_Not_Found, "Page not found");

    auto server = asyncgi::Server{io, router};
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

</details>

### Route parameters

When using `asyncgi::Router` with regular expressions, request processors must satisfy
the `ParametrizedRequestProcessor` requirement. That means that a function object must be callable with the
arguments `const TRouteParams&..., const asyncgi::Request&, asyncgi::Response&`, and have a return type of `void`.
The `TRouteParams` represents zero or more parameters generated from the capturing groups of the regular expression. For
example, `void (int age, string name, const asyncgi::Request&, asyncgi::Response&)` signature can be used to process
requests matched by `asyncgi::rx{"/person/(\\w+)/age/(\\d+)"}`.

In the following example a `ParametrizedRequestProcessor` named `GuestBookRemoveMessage` is added to remove the stored
guest book messages:


<details>
  <summary>Example</summary>

```c++
///examples/example_route_params.cpp
///
#include <asyncgi/asyncgi.h>
#include <mutex>

using namespace asyncgi;
using namespace std::string_literals;

class GuestBookState {
public:
    std::vector<std::string> messages()
    {
        auto lock = std::scoped_lock{mutex_};
        return messages_;
    }

    void addMessage(const std::string& msg)
    {
        auto lock = std::scoped_lock{mutex_};
        messages_.emplace_back(msg);
    }

    void removeMessage(int index)
    {
        auto lock = std::scoped_lock{mutex_};
        if (index < 0 || index >= static_cast<int>(messages_.size()))
            return;
        messages_.erase(std::next(messages_.begin(), index));
    }

private:
    std::vector<std::string> messages_;
    std::mutex mutex_;
};

std::string makeMessage(int index, const std::string& msg)
{
    return msg + R"(<form action="/delete/)" + std::to_string(index) +
            R"(" method="post"> <input value="Delete" type="submit"> </form></div>)";
}

class GuestBookPage {
public:
    explicit GuestBookPage(GuestBookState& state)
        : state_{&state}
    {
    }

    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        auto messages = state_->messages();
        auto page = "<h1>Guest book</h1>"s;
        if (messages.empty())
            page += "<p>No messages</p>";
        else
            for (auto i = 0; i < static_cast<int>(messages.size()); ++i)
                page += "<p>" + makeMessage(i, messages.at(i)) + "</p>";

        page += "<hr>";
        page += "<form method=\"post\" enctype=\"multipart/form-data\">"
                "<label for=\"msg\">Message:</label>"
                "<input id=\"msg\" name=\"msg\" value=\"\">"
                "<input value=\"Submit\" data-popup=\"true\" type=\"submit\">"
                "</form>";
        response.send(page);
    }

private:
    GuestBookState* state_;
};

class GuestBookAddMessage {
public:
    explicit GuestBookAddMessage(GuestBookState& state)
        : state_{&state}
    {
    }

    void operator()(const asyncgi::Request& request, asyncgi::Response& response)
    {
        state_->addMessage(std::string{request.formField("msg")});
        response.redirect("/");
    }

private:
    GuestBookState* state_;
};

class GuestBookRemoveMessage {
public:
    explicit GuestBookRemoveMessage(GuestBookState& state)
        : state_{&state}
    {
    }

    void operator()(int index, const asyncgi::Request&, asyncgi::Response& response)
    {
        state_->removeMessage(index);
        response.redirect("/");
    }

private:
    GuestBookState* state_;
};

int main()
{
    auto io = asyncgi::IO{4};
    auto state = GuestBookState{};
    auto router = asyncgi::Router{};
    router.route("/", http::RequestMethod::Get).process<GuestBookPage>(state);
    router.route("/", http::RequestMethod::Post).process<GuestBookAddMessage>(state);
    router.route(asyncgi::rx{"/delete/(.+)"}, http::RequestMethod::Post).process<GuestBookRemoveMessage>(state);
    router.route().set(http::ResponseStatus::_404_Not_Found, "Page not found");

    auto server = asyncgi::Server{io, router};
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

</details>

Regular expression capture groups are transformed into request processor arguments using `std::stringstream`. In order
to support request processors with user-defined parameter types, it is necessary to provide a specialization
of `asyncgi::config::StringConverter` class template. The previous example has been modified to reformat
the `GuestBookRemoveMessage` request processor to use the `MessageNumber` structure as a request processor argument:

<details>
  <summary>Example</summary>

```c++
///examples/example_route_params_user_defined_types.cpp
///
#include <asyncgi/asyncgi.h>
#include <mutex>

using namespace asyncgi;
using namespace std::string_literals;

struct MessageNumber {
    int value;
};

template<>
struct asyncgi::config::StringConverter<MessageNumber> {
    static std::optional<MessageNumber> fromString(const std::string& data)
    {
        return MessageNumber{std::stoi(data)};
    }
};

class GuestBookState {
public:
    std::vector<std::string> messages()
    {
        auto lock = std::scoped_lock{mutex_};
        return messages_;
    }

    void addMessage(const std::string& msg)
    {
        auto lock = std::scoped_lock{mutex_};
        messages_.emplace_back(msg);
    }

    void removeMessage(int index)
    {
        auto lock = std::scoped_lock{mutex_};
        if (index < 0 || index >= static_cast<int>(messages_.size()))
            return;
        messages_.erase(std::next(messages_.begin(), index));
    }

private:
    std::vector<std::string> messages_;
    std::mutex mutex_;
};

std::string makeMessage(int index, const std::string& msg)
{
    return msg + R"(<form action="/delete/)" + std::to_string(index) +
            R"(" method="post"> <input value="Delete" type="submit"> </form></div>)";
}

class GuestBookPage {
public:
    explicit GuestBookPage(GuestBookState& state)
        : state_{&state}
    {
    }

    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        auto messages = state_->messages();
        auto page = "<h1>Guest book</h1>"s;
        if (messages.empty())
            page += "<p>No messages</p>";
        else
            for (auto i = 0; i < static_cast<int>(messages.size()); ++i)
                page += "<p>" + makeMessage(i, messages.at(i)) + "</p>";

        page += "<hr>";
        page += "<form method=\"post\" enctype=\"multipart/form-data\">"
                "<label for=\"msg\">Message:</label>"
                "<input id=\"msg\" name=\"msg\" value=\"\">"
                "<input value=\"Submit\" data-popup=\"true\" type=\"submit\">"
                "</form>";
        response.send(page);
    }

private:
    GuestBookState* state_;
};

class GuestBookAddMessage {
public:
    explicit GuestBookAddMessage(GuestBookState& state)
        : state_{&state}
    {
    }

    void operator()(const asyncgi::Request& request, asyncgi::Response& response)
    {
        state_->addMessage(std::string{request.formField("msg")});
        response.redirect("/");
    }

private:
    GuestBookState* state_;
};

class GuestBookRemoveMessage {
public:
    explicit GuestBookRemoveMessage(GuestBookState& state)
        : state_{&state}
    {
    }

    void operator()(MessageNumber msgNumber, const asyncgi::Request&, asyncgi::Response& response)
    {
        state_->removeMessage(msgNumber.value);
        response.redirect("/");
    }

private:
    GuestBookState* state_;
};

int main()
{
    auto io = asyncgi::IO{4};
    auto state = GuestBookState{};
    auto router = asyncgi::Router{};
    router.route("/", http::RequestMethod::Get).process<GuestBookPage>(state);
    router.route("/", http::RequestMethod::Post).process<GuestBookAddMessage>(state);
    router.route(asyncgi::rx{"/delete/(.+)"}, http::RequestMethod::Post).process<GuestBookRemoveMessage>(state);
    router.route().set(http::ResponseStatus::_404_Not_Found, "Page not found");

    auto server = asyncgi::Server{io, router};
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

</details>

### Route context

When using `asyncgi::Router`, it is possible to specify a template argument for a context structure type. This
structure is then passed to the `ContextualRequestProcessor` functions and can be accessed and modified throughout the
request processing for multiple routes. The `ContextualRequestProcessor` is a `RequestProcessor` that takes an
additional argument referring to the context object.  
A single request can match multiple routes, as long as all preceding request processors do not send any response. This
allows using `asyncgi::Router` to register middleware-like processors, which primarily modify the route context for
subsequent processors.

The next example demonstrates how a route context can be used for storing authorization information:

<details>
  <summary>Example</summary>

```c++
///examples/example_route_context.cpp
///
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
    void operator()(const asyncgi::Request&, asyncgi::Response& response, RouteContext& context)
    {
        if (context.role == AccessRole::Guest)
            response.send(R"(
                    <html>
                    <form method="post" enctype="multipart/form-data">
                    <label for="msg">Login:</label>
                    <input id="login" name="login" value="">
                    <label for="msg">Password:</label>
                    <input id="passwd" name="passwd" value="">
                    <input value="Submit" data-popup="true" type="submit">
                    </form></html>)");
        else //We are already logged in as the administrator
            response.redirect("/");
    }
};

struct LoginPageAuthorize {
    void operator()(const asyncgi::Request& request, asyncgi::Response& response, RouteContext& context)
    {
        if (context.role == AccessRole::Guest) {
            if (request.formField("login") == "admin" && request.formField("passwd") == "12345")
                response.redirect(
                        "/",
                        asyncgi::http::RedirectType::Found,
                        {asyncgi::http::Cookie("admin_id", "ADMIN_SECRET")});
            else
                response.redirect("/login");
        }
        else //We are already logged in as the administrator
            response.redirect("/");
    }
};

int main()
{
    auto io = asyncgi::IO{4}; //4 threads processing requests
    auto router = asyncgi::Router<RouteContext>{};
    router.route(asyncgi::rx{".*"}).process<AdminAuthorizer>();
    router.route("/").process(
            [](const asyncgi::Request&, asyncgi::Response& response, RouteContext& context)
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
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

</details>

### Route matchers

Any parameter of request, response and context objects can be registered for route matching
in `asyncgi::Router::route()` method. To achieve this, it is required to provide a specialization of
the `asyncgi::config::RouteMatcher` class template and implement a comparator bool operator() within it. Let's see how
to register the enum class `Access` from the previous example as a route matcher:

<details>
  <summary>Example</summary>

```c++
///examples/example_route_matcher.cpp
///
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
    auto io = asyncgi::IO{4};
    auto router = asyncgi::Router<RouteContext>{};
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

    auto server = asyncgi::Server{io, router};
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

</details>

### Complete guest book example

Let's combine all the previous examples to create a simple guest book.  
The messages in the guest book will only persist during the application runtime, as they are stored in a `std::vector`.
The admin credentials for logging in are as follows: login: `admin`, password: `12345`. The admin account has the
ability to delete posts.

<details>
  <summary>Example</summary>

```c++
#include <asyncgi/asyncgi.h>
#include <mutex>
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
    bool operator()(AccessRole value, const asyncgi::Request&, asyncgi::Response&, RouteContext& context) const
    {
        return value == context.role;
    }
};

void authorizeAdmin(const asyncgi::Request& request, asyncgi::Response&, RouteContext& context)
{
    if (request.cookie("admin_id") == "ADMIN_SECRET")
        context.role = AccessRole::Admin;
}

void showLoginPage(const asyncgi::Request&, asyncgi::Response& response)
{
    response.send(R"(
            <head><link rel="stylesheet" href="https://cdn.simplecss.org/simple.min.css"></head>
            <form method="post" enctype="multipart/form-data">
                <label for="msg">Login:</label>
                <input id="login" name="login" value="">
                <label for="msg">Password:</label>
                <input id="passwd" name="passwd" value="">
                <input value="Submit" data-popup="true" type="submit">
            </form>)");
}

void loginAdmin(const asyncgi::Request& request, asyncgi::Response& response)
{
    if (request.formField("login") == "admin" && request.formField("passwd") == "12345")
        response.redirect("/", http::RedirectType::Found, {http::Cookie("admin_id", "ADMIN_SECRET")});
    else
        response.redirect("/login");
}

void logoutAdmin(const asyncgi::Request&, asyncgi::Response& response)
{
    response.redirect("/", http::RedirectType::Found, {http::Cookie("admin_id", "")});
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
    return [&state](const asyncgi::Request& request, asyncgi::Response& response, RouteContext& context)
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
        response.send(page);
    };
}

auto addMessage(GuestBookState& state)
{
    return [&state](const asyncgi::Request& request, asyncgi::Response& response)
    {
        if (std::all_of(
                    request.formField("msg").begin(),
                    request.formField("msg").end(),
                    [](char ch)
                    {
                        return std::isspace(static_cast<unsigned char>(ch));
                    }))
            response.redirect("/?error=empty_msg");
        else if (
                request.formField("msg").find("http://") != std::string_view::npos ||
                request.formField("msg").find("https://") != std::string_view::npos)
            response.redirect("/?error=urls_in_msg");
        else {
            state.addMessage(std::string{request.formField("name")}, std::string{request.formField("msg")});
            response.redirect("/");
        }
    };
}

auto removeMessage(GuestBookState& state)
{
    return [&state](int index, const asyncgi::Request&, asyncgi::Response& response)
    {
        state.removeMessage(index);
        response.redirect("/");
    };
}

int main()
{
    auto io = asyncgi::IO{4};
    auto state = GuestBookState{};
    auto router = asyncgi::Router<RouteContext>{};
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
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

</details>

The live demo can be accessed [here](https://asyncgi-guestbook.eelnet.org).

### Timer

A timer object `asyncgi::Timer` can be created to change or check some state periodically.

<details>
  <summary>Example</summary>

```c++
///examples/example_timer.cpp
///
#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

struct Greeter{
    Greeter(const int& secondsCounter)
        : secondsCounter_{&secondsCounter}
    {
    }

    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        response.send("Hello world\n(alive for " + std::to_string(*secondsCounter_) + " seconds)");
    }

private:
    const int* secondsCounter_;
};

int main()
{
    auto io = asyncgi::IO{};
    int secondsCounter = 0;

    auto timer = asyncgi::Timer{io};
    timer.startPeriodic(
            std::chrono::seconds(1),
            [&secondsCounter]()
            {
                ++secondsCounter;
            });

    auto router = asyncgi::Router{};
    router.route("/").process<Greeter>(secondsCounter);
    router.route().set(http::ResponseStatus::_404_Not_Found);

    auto server = asyncgi::Server{io, router};
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

</details>


The `asyncgi::Timer::waitFuture` method can accept an `std::future` object and invoke a provided callable object with
its result when the future object becomes ready. This function does not block while waiting and uses an internal timer
to periodically check the state of the future. To use it during request processing, a timer object created from
an `asyncgi::Response` reference must be used. It is important to avoid using this timer after the response has already
been sent.

<details>
  <summary>Example</summary>

```c++
///examples/response_wait_future.cpp
///
#include <asyncgi/asyncgi.h>
#include <thread>

using namespace asyncgi;

struct DelayedPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        auto timer = asyncgi::Timer{response};
        timer.waitFuture(
                std::async(
                        std::launch::async,
                        []
                        {
                            std::this_thread::sleep_for(std::chrono::seconds(3));
                            return "world";
                        }),
                [response](const std::string& result) mutable
                {
                    response.send(http::Response{"Hello " + result});
                });
    }
};

int main()
{
    auto io = asyncgi::IO{};
    auto router = asyncgi::Router{};
    auto delayedPage = DelayedPage{};
    router.route("/", http::RequestMethod::Get).process(delayedPage);
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = asyncgi::Server{io, router};
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

</details>

### Client

With `asyncgi::Client` it's possible to make direct requests to `FastCGI` applications. This
enables multiple `asyncgi`-based applications to communicate with each other without the need for other inter-process
communication solutions.

<details>
  <summary>Example</summary>

```c++
///examples/example_client.cpp
///
#include <asyncgi/asyncgi.h>
#include <iostream>

using namespace asyncgi;

int main()
{
    auto io = asyncgi::IO{};
    auto client = asyncgi::Client{io};
    client.makeRequest(
            "/tmp/fcgi.sock",
            http::Request{http::RequestMethod::Get, "/"},
            [&io](const std::optional<http::ResponseView>& response)
            {
                if (response)
                    std::cout << response->body() << std::endl;
                else
                    std::cout << "No response" << std::endl;
                io.stop();
            });
    io.run();
}
```
</details>

To make FastCGI requests during request processing, a client object created from an `asyncgi::Response` reference must
be used. It is important to avoid using this client object after the response has already been sent.

<details>
  <summary>Example</summary>

```c++
///examples/example_client_in_processor.cpp
///
#include <asyncgi/asyncgi.h>

namespace http = asyncgi::http;

struct RequestPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        // making request to FastCgi application listening on /tmp/fcgi.sock and showing the received response
        auto client = asyncgi::Client{response};
        client.makeRequest(
                "/tmp/fcgi.sock",
                http::Request{http::RequestMethod::Get, "/"},
                [response](const std::optional<http::ResponseView>& reqResponse) mutable
                {
                    if (reqResponse)
                        response.send(std::string{reqResponse->body()});
                    else
                        response.send("No response");
                });
    }
};

int main()
{
    auto io = asyncgi::IO{};
    auto router = asyncgi::Router{};
    router.route("/", http::RequestMethod::Get).process<RequestPage>();
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = asyncgi::Server{io, router};
    server.listen("/tmp/fcgi_client.sock");
    io.run();
}
```

</details>


### Executing an asio task

`asyncgi` internally uses the `asio` library. A dispatcher object `asyncgi::AsioDispatcher` can be created to invoke
callable objects that require access to the `asio::io_context` object.

<details>
  <summary>Example</summary>

```c++
///examples/example_asio_dispatcher.cpp
///
#include <asyncgi/asyncgi.h>
#include <asio/steady_timer.hpp>
#include <iostream>

int main()
{
    auto io = asyncgi::IO{};
    auto disp = asyncgi::AsioDispatcher{io};
    disp.postTask(
            [&io](const asyncgi::TaskContext& ctx) mutable
            {
                auto timer = std::make_shared<asio::steady_timer>(ctx.io());
                timer->expires_after(std::chrono::seconds{3});
                timer->async_wait(
                        [timer, ctx, &io](auto&) mutable
                        {
                            std::cout << "Hello world" << std::endl;
                            io.stop();
                        });
            });
    io.run();
}
```

</details>

To invoke such a callable object during request processing, a dispatcher created from an `asyncgi::Response` reference
must be used. It is important to avoid using this dispatcher after the response has already been sent.

<details>
  <summary>Example</summary>

```c++
///examples/example_response_dispatching_asio_task.cpp
///
#include <asyncgi/asyncgi.h>
#include <asio/steady_timer.hpp>

namespace http = asyncgi::http;

struct DelayedPage{
    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        auto disp = asyncgi::AsioDispatcher{response};
        disp.postTask(
                [response](const asyncgi::TaskContext& ctx) mutable
                {
                    auto timer = std::make_shared<asio::steady_timer>(ctx.io());
                    timer->expires_after(std::chrono::seconds{3});
                    timer->async_wait([timer, response, ctx](auto&) mutable { // Note how we capture ctx object here,
                        response.send("Hello world"); // it's necessary to keep it (or its copy) alive
                    }); // before the end of request processing
                });
    }
};

int main()
{
    auto io = asyncgi::IO{};
    auto router = asyncgi::Router{};
    router.route("/", http::RequestMethod::Get).process<DelayedPage>();
    router.route().set(http::ResponseStatus::_404_Not_Found);
    auto server = asyncgi::Server{io, router};
    server.listen("/tmp/fcgi.sock");
    io.run();
}
```

</details>

To use `asyncgi` with the `Boost.Asio` library, set the `ASYNCGI_USE_BOOST_ASIO` CMake variable .

## Development status

`asyncgi` is currently in the open beta stage, with all planned features complete. Until it reaches a non-zero major
version number, there may be frequent introductions of backward-incompatible changes.

Unit tests are not included because most of the functionality in `asyncgi` is derived from the following libraries,
which already have their own test coverage:

* [asio](https://github.com/chriskohlhoff/asio) - used for establishing connections, sending and receiving data.
* [fcgi_responder](https://github.com/kamchatka-volcano/fcgi_responder/) - implementation of the `FastCGI` protocol.
* [whaleroute](https://github.com/kamchatka-volcano/whaleroute/) - implementation of the request router.
* [hot_teacup](https://github.com/kamchatka-volcano/hot_teacup/) - parsing of HTTP data received over `FastCGI`
  connections, forming HTTP responses.

Instead of mocking code that integrates the functionality of these libraries, `asyncgi` is tested using functional
tests. These tests check the behavior of the executables found in the `examples/` directory when running with the NGINX
server. You can find these tests in the `functional_tests/` directory.

## Installation

Download and link the library from your project's CMakeLists.txt:
```
cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(cmdlime
    GIT_REPOSITORY "https://github.com/kamchatka-volcano/asyncgi.git"
    GIT_TAG "origin/master"
)
#uncomment if you need to install cmdlime with your target
#set(INSTALL_ASYNCGI ON)
FetchContent_MakeAvailable(asyncgi)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE asyncgi::asyncgi)
```

To install the library system-wide, use the following commands:
```
git clone https://github.com/kamchatka-volcano/asyncgi.git
cd asyncgi
cmake -S . -B build
cmake --build build
cmake --install build
```

After installation, you can use the find_package() command to make the installed library available inside your project:

```
find_package(asyncgi 0.1.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE asyncgi::asyncgi)   
```

If you want to use the `Boost.Asio` library, `Boost` dependencies can be resolved
using [vcpkg](https://vcpkg.io/en/getting-started.html) by running the build with this command:

```
cmake -S . -B build -DASYNCGI_USE_BOOST_ASIO=ON -DCMAKE_TOOLCHAIN_FILE=<vcpkg path>/scripts/buildsystems/vcpkg.cmake
```

## Building examples

```
cd asyncgi
cmake -S . -B build -DENABLE_EXAMPLES=ON
cmake --build build
cd build/examples
```

## Running functional tests

Download [`lunchtoast`](https://github.com/kamchatka-volcano/lunchtoast/releases) executable, build `asyncgi` examples
and start NGINX with `functional_tests/nginx_*.conf` config file.
Launch tests with the following command:

* Linux command:

```
lunchtoast functional_tests
```

* Windows command:

```
lunchtoast.exe functional_tests -shell="msys2 -c" -skip=linux
```

To run functional tests on Windows, it's recommended to use the bash shell from the `msys2` project. After installing
it, add the following script `msys2.cmd` to your system `PATH`:

```bat
@echo off
setlocal
IF NOT DEFINED MSYS2_PATH_TYPE set MSYS2_PATH_TYPE=inherit
set CHERE_INVOKING=1
C:\\msys64\\usr\\bin\\bash.exe -leo pipefail %*
```

## License

**asyncgi** is licensed under the [MS-PL license](/LICENSE.md)  






