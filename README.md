<p align="center">
  <img height="128" src="doc/logo.png"/>
</p>

**asyncgi** - is a C++17 asynchronous microframework for creating web applications interfacing with any HTTP server supporting [FastCGI](https://en.wikipedia.org/wiki/FastCGI) protocol. It aims to provide a modern way of using [CGI](https://en.wikipedia.org/wiki/Common_Gateway_Interface), with a custom performant FastCGI implementation in C++, multithreading support and a clean and simple API:
 
```c++
#include <asyncgi/asyncgi.h>
using namespace std::string_literals;

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::GET).set("Hello world"s);
    router.route().set(http::ResponseStatus::Code_404_Not_Found);
   
    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}
```

## Table of Contents
*    [Usage](#usage)
     * [Connection](#connection)
     * [Request processor](#request-processor)
     * [Router](#router) 
     * [Route context](#route-context)
     * [Route specification](#route-specification)
     * [Timer](#timer)
     * [Client](#client)
     * [Waiting for future](#waiting-for-future)
     * [Executing an asio task](#executing-an-asio-task)
*    [Development status](#development-status)     
*    [Installation](#installation)
*    [Building examples](#building-examples)
*    [License](#license)

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

To process requests it's necessary to provide an implementation of `asyncgi::RequestProcessor` class and its method `void process(const asyncgi::Request&, asyncgi::Response&)`.

<details>
  <summary>Example</summary>

```c++
///examples/example_request_processor.cpp
///
#include <asyncgi/asyncgi.h>

using namespace std::string_literals;
struct Greeter : asyncgi::RequestProcessor<>{
    void process(
            const asyncgi::Request& request,
            asyncgi::Response<>& response) override
    {
        if (request.path() == "/")
            response.send("Hello world"s);
        else
            response.send(http::ResponseStatus::Code_404_Not_Found);
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto greeter = Greeter{};
    auto server = app->makeServer(greeter);
    //Listen for FastCGI connections on UNIX domain socket
    server->listen("/tmp/fcgi.sock");
    //or over TCP
    //server->listen("127.0.0.1", 9000);
    app->exec();
    return 0;
}
```
</details>

### Router  
Multiple request processors can be registered in `asyncgi::Router` object matching them to the paths specified in requests. `asyncgi::Router` by itself is a request processor and inherits from `asyncgi::RequestProcessor` class. 
To use multiple threads for request processing, pass a desired number of workers to `asyncgi::makeApp()` factory function.  In that case any shared data in request processors must be protected from the concurrent read/write access by the user.
<details>
  <summary>Example</summary>

```c++
///examples/example_router.cpp
///
#include <asyncgi/asyncgi.h>
#include <mutex>

using namespace std::string_literals;

class State {
public:
    std::string name()
    {
        auto lock = std::scoped_lock{mutex_};
        return name_;
    }

    void setName(std::string name)
    {
        auto lock = std::scoped_lock{mutex_};
        name_ = std::move(name);
    }

private:
    std::string name_;
    std::mutex mutex_;
};

class HelloPage : public asyncgi::RequestProcessor<>{
public:
    HelloPage(State& state)
        : state_(state)
    {}

    void process(const asyncgi::Request&, asyncgi::Response<>& response) override
    {
        auto name = state_.name();
        if (name.empty())
            name = std::string{"world"};

        response.send("<html>"
                      "<p>Hello, " + name + "</p>"
                      "<a href=\"/settings\">settings</a>"
                      "</html>");
    }
private:
    State& state_;
};

class ChangeSettings : public asyncgi::RequestProcessor<>{
public:
    ChangeSettings(State& state)
        : state_(state)
    {}
    void process(const asyncgi::Request& request, asyncgi::Response<>& response) override
    {
        state_.setName(std::string{request.formField("name")});
        response.send(http::Response("/", http::RedirectType::Found));
    }

private:
    State& state_;
};

int main()
{
    auto app = asyncgi::makeApp(4); //4 threads processing requests
    auto state = State{};
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::GET).process<HelloPage>(state);
    router.route("/settings", http::RequestMethod::POST).process<ChangeSettings>(state);
    router.route("/settings", http::RequestMethod::GET).process(
            [](const auto&, auto& response){
                response.send(
                         "<html>"
                         "<form method=\"post\" enctype=\"multipart/form-data\">"
                         "<label for=\"name\">Name:</label>"
                         "<input id=\"name\" name=\"name\" value=\"\">"
                         "<input value=\"Submit\" data-popup=\"true\" type=\"submit\">"
                         "</form>"
                         "</html>"s);
            }
    );
    router.route().set(http::ResponseStatus::Code_404_Not_Found);

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}
```
</details>

### Route context  
`asyncgi::Route`, `asyncgi::RequestProcessor`, and `asyncgi::Response` can take a template argument with a type of structure that is stored in `asyncgi::Response` object and can be accessed and changed during request processing across multiple routes. A single request can match multiple routes if all invoked request processors prior the last one don't send any response. This allows us to use `asyncgi::Router` for registering middleware-like processors that only modify the route context to be used in the following processors.   

<details>
  <summary>Example</summary>

```c++
///examples/example_route_context.cpp
///
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
    void process(const asyncgi::Request& request, asyncgi::Response<RouteContext>& response) override
    {
        if (response.context().access == Access::Authorized)
            response.send("Welcome, admin!"s);
        else
            response.send(http::Response{http::ResponseStatus::Code_401_Unauthorized, "You are not authorized to view this page."s});
    }
};

struct ModerationPage : asyncgi::RequestProcessor<RouteContext>{
    void process(const asyncgi::Request& request, asyncgi::Response<RouteContext>& response) override
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
```
</details>

### Route specification

Any parameter of request or response objects (including parameters of the route context available in the response object) can be registered to be used for route matching in `asyncgi::Router::route()` method. To do this, provide the specialization of the class template `asyncgi::traits::RouteSpecification` and implement a comparator `bool operator()` inside it. See how to register `enum class Access` from the previous example as a route specification:

<details>
  <summary>Example</summary>

```c++
///examples/example_route_specification.cpp
///
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
    void process(const asyncgi::Request& request, asyncgi::Response<RouteContext>& response) override
    {
       response.send("Welcome, admin!"s);
    }
};

struct ModerationPage : asyncgi::RequestProcessor<RouteContext>{
    void process(const asyncgi::Request& request, asyncgi::Response<RouteContext>& response) override
    {
        response.send("Welcome, moderator!"s);
    }
};

template<>
struct asyncgi::traits::RouteSpecification<Access, asyncgi::Request, asyncgi::Response<RouteContext>> {
    bool operator()(Access value, const asyncgi::Request&, asyncgi::Response<RouteContext>& response) const
    {
        return value == response.context().access;
    }
};


int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter<RouteContext>();
    router.route(std::regex{".*"}).process<Authorizer>();
    router.route("/admin", http::RequestMethod::GET, Access::Authorized).process<AdminPage>();
    //RequestMethod parameter is implemented using RouteSpecification as well.
    //As you can see below, the order of provided RouteSpecification parameters isn't important.
    router.route("/moderation", Access::Authorized, http::RequestMethod::GET).process<ModerationPage>();
    router.route(std::regex{".*"}, Access::Forbidden).set(http::Response{http::ResponseStatus::Code_401_Unauthorized, "You are not authorized to view this page."s});
    router.route().set(http::Response{http::ResponseStatus::Code_404_Not_Found, "Page not found."});

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}
```
</details>

### Timer

A timer object implementing the interface `asyncgi::ITimer` can be created to change or check some state periodically.

<details>
  <summary>Example</summary>

```c++
///examples/example_timer.cpp
///
#include <asyncgi/asyncgi.h>

using namespace std::string_literals;
struct Greeter : asyncgi::RequestProcessor<>{
    Greeter(const int& secondsCounter)
        : secondsCounter_{secondsCounter}
    {
    }

    void process(const asyncgi::Request& request, asyncgi::Response<>& response) override
    {
        response.send("Hello world\n(alive for "s + std::to_string(secondsCounter_) + " seconds)"s);
    }

private:
    const int& secondsCounter_;
};

int main()
{
    auto app = asyncgi::makeApp();
    int secondsCounter = 0;

    auto timer = app->makeTimer();
    timer->start(std::chrono::seconds(1), [&secondsCounter](){
        ++secondsCounter;
    }, asyncgi::TimerMode::Repeatedly);

    auto router = asyncgi::makeRouter();
    router.route("/").process<Greeter>(secondsCounter);
    router.route().set(http::ResponseStatus::Code_404_Not_Found);

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}
```
</details>

### Client
A client object implementing the interface `asyncgi::IClient` can be created to make direct requests to `FastCGI` applications. This way several `asyncgi` based applications can communicate with each other without using any other solution for inter-process communication.

<details>
  <summary>Example</summary>

```c++
///examples/example_client.cpp
///
#include <asyncgi/asyncgi.h>
#include <iostream>

int main()
{
    auto app = asyncgi::makeApp();
    auto client = app->makeClient();
    client->makeRequest("/tmp/fcgi.sock", http::Request{http::RequestMethod::GET, "/"},
            [](const std::optional<http::ResponseView>& response){
                if (response)
                    std::cout << response->body() << std::endl;
                else
                    std::cout << "No response" << std::endl;
            }
    );
    app->exec();
    return 0;
}
```
</details>

If you need to make a `FastCGI` request during request processing don't use a client object directly and instead invoke an `asyncgi::Response::makeRequest()` method.

<details>
  <summary>Example</summary>

```c++
///examples/example_client_in_processor.cpp
///
#include <asyncgi/asyncgi.h>

using namespace std::string_literals;
struct RequestPage : asyncgi::RequestProcessor<>{
    void process(const asyncgi::Request& request, asyncgi::Response<>& response) override
    {
        //making request to FastCgi application listening on 127.0.0.1:9000 and showing the received response
        response.makeRequest("127.0.0.1", 9000, http::Request{http::RequestMethod::GET, "/"},
                [response](const std::optional<http::ResponseView>& reqResponse) mutable {
                 if (reqResponse)
                     response.send(std::string{reqResponse->body()});
                 else
                     response.send("No response"s);
                }
        );
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::GET).process<RequestPage>();
    router.route().set(http::ResponseStatus::Code_404_Not_Found);
    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}
```
</details>

---
*The next features were implemented for possible interoperability with other libraries and extension of `asyncgi` functionality, but they aren't well-thought-out, and currently there's no much confidence in their applicability. They're the first contenders to be changed significantly or even be removed.*


### Waiting for future 
`asyncgi::Response::waitFuture` can take an `std::future` object and invoke its result on provided callable object when this future object is ready. It doesn't block while waiting and the internal timer is used to check the future state periodically.
<details>
  <summary>Example</summary>

```c++
///examples/response_wait_future.cpp
///
#include <asyncgi/asyncgi.h>

using namespace std::string_literals;
struct DelayedPage : asyncgi::RequestProcessor<>{
    void process(const asyncgi::Request& request, asyncgi::Response<>& response) override
    {
        response.waitFuture(
            std::async(std::launch::async, []{std::this_thread::sleep_for(std::chrono::seconds(3)); return "World"s;}),
            [response](const std::string& result) mutable
            {
                response.send(http::Response{"Hello " + result});
            });
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::GET).process<DelayedPage>();
    router.route().set(http::ResponseStatus::Code_404_Not_Found);
    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}
```
</details>

### Executing an asio task
`asyncgi` uses `asio` library internally. A dispatcher object, implementing the interface `asyncgi::IAsioDispatcher`, can be created to invoke callable objects required to have access to `asio::io_context` object.

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
    auto app = asyncgi::makeApp();
    auto disp = app->makeAsioDispatcher();
    disp->postTask([](const asyncgi::TaskContext& ctx) mutable
                {
                    auto timer = std::make_shared<asio::steady_timer>(ctx.io());
                    timer->expires_after(std::chrono::seconds{3});
                    timer->async_wait([timer, ctx](auto& ec) mutable{
                        std::cout << "Hello world" << std::endl;
                    });
                });
    app->exec();
    return 0;
}
```
</details>

If you need to invoke such callable object during request processing don't use a dispatcher object directly and instead invoke an `asyncgi::Response::executeTask()` method.

<details>
  <summary>Example</summary>

```c++
///examples/example_response_dispatching_asio_task.cpp
///
#include <asyncgi/asyncgi.h>
#include <asio/steady_timer.hpp>

using namespace std::string_literals;
struct DelayedPage : asyncgi::RequestProcessor<>{
    void process(const asyncgi::Request& request, asyncgi::Response<>& response) override
    {
        response.executeTask(
                [response](const asyncgi::TaskContext& ctx) mutable
                {
                    auto timer = std::make_shared<asio::steady_timer>(ctx.io());
                    timer->expires_after(std::chrono::seconds{3});
                    timer->async_wait([timer, response, ctx](auto& ec) mutable{ //Note how we capture ctx object here,
                        response.send("Hello world"s);                          //it's necessary to keep it (or its copy) alive
                    });                                                         //before the end of request processing
                });
    }
};

int main()
{
    auto app = asyncgi::makeApp();
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::GET).process<DelayedPage>();
    router.route().set(http::ResponseStatus::Code_404_Not_Found);
    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}
```
</details>

## Development status 

`asyncgi` can be considered to be in the open beta stage - the all planed features are completed, but there are many areas of improvement. Until it gets the non-zero major version number, the backwards incompatible changes may be introduced quite often.  

There are currently no tests, and there are no plans of adding unit tests so far, because most of `asyncgi` functionality comes from the following libraries already covered with unit tests:
* [asio](https://github.com/chriskohlhoff/asio) - used for establishing connections, sending and receiving data.
* [fcgi_responder](https://github.com/kamchatka-volcano/fcgi_responder/) - implementation of the `FastCGI` protocol.
* [whaleroute](https://github.com/kamchatka-volcano/whaleroute/) - implementation of the request router.
* [hot_teacup](https://github.com/kamchatka-volcano/hot_teacup/) - parsing of HTTP data received over `FastCGI` connections, forming HTTP responses.

Basically `asyncgi` is glue code holding all these libraries together behind a convenient interface. It makes more sense to provide functionality tests based on the `examples` directory to check overall expected behaviour, and it probably will be done in the future.


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

For the system-wide installation use these commands:
```
git clone https://github.com/kamchatka-volcano/asyncgi.git
cd asyncgi
cmake -S . -B build
cmake --build build
cmake --install build
```

Afterwards, you can use find_package() command to make installed library available inside your project:
```
find_package(asyncgi 0.1.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE asyncgi::asyncgi)   
```


## Building examples
```
cd asyncgi
cmake -S . -B build -DENABLE_EXAMPLES=ON
cmake --build build
cd build/examples
```

## License
**asyncgi** is licensed under the [MS-PL license](/LICENSE.md)  






