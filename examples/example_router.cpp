#include <asyncgi/asyncgi.h>
#include <mutex>

using namespace asyncgi;

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

class HelloPage{
public:
    HelloPage(State& state)
        : state_(state)
    {}

    void operator()(const asyncgi::Request&, asyncgi::Response& response)
    {
        auto name = state_.name();
        if (name.empty())
            name = std::string{"world"};

        response.send(
                "<html>"
                "<p>Hello, " +
                name +
                "</p>"
                "<a href=\"/settings\">settings</a>"
                "</html>");
    }

private:
    State& state_;
};

class ChangeSettings{
public:
    ChangeSettings(State& state)
        : state_(state)
    {}

    void operator()(const asyncgi::Request& request, asyncgi::Response& response)
    {
        state_.setName(std::string{request.formField("name")});
        response.redirect("/");
    }

private:
    State& state_;
};

int main()
{
    auto app = asyncgi::makeApp(4); //4 threads processing requests
    auto state = State{};
    auto router = asyncgi::makeRouter();
    router.route("/", http::RequestMethod::Get).process<HelloPage>(state);
    router.route("/settings", http::RequestMethod::Post).process<ChangeSettings>(state);
    router.route("/settings", http::RequestMethod::Get).process(
                    [](const asyncgi::Request&, asyncgi::Response& response)
                    {
                        response.send("<html>"
                                      "<form method=\"post\" enctype=\"multipart/form-data\">"
                                      "<label for=\"name\">Name:</label>"
                                      "<input id=\"name\" name=\"name\" value=\"\">"
                                      "<input value=\"Submit\" data-popup=\"true\" type=\"submit\">"
                                      "</form>"
                                      "</html>");
                    });
    router.route().set(http::Response{http::ResponseStatus::_404_Not_Found, "Page not found"});
    //Alternatively, it's possible to pass arguments for creation of http::Response object to the set() method.
    //router.route().set(http::ResponseStatus::Code_404_Not_Found, "Page not found");

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}