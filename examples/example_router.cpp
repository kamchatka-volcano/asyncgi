#include <asyncgi/asyncgi.h>
#include <mutex>

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

        auto page = "<html>"
                    "<p>Hello, " + name + "</p>"
                    "<a href=\"/settings\">settings</a>"
                    "</html>";
        response.send(http::Response::Content(page));
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
        response.send(http::Response::Redirect("/"));
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
                response.send(http::Response::Content(
                         "<html>"
                         "<form method=\"post\" enctype=\"multipart/form-data\">"
                         "<label for=\"name\">Name:</label>"
                         "<input id=\"name\" name=\"name\" value=\"\">"
                         "<input value=\"Submit\" data-popup=\"true\" type=\"submit\">"
                         "</form>"
                         "</html>"));
            }
    );
    router.route().set(http::ResponseStatus::Code_404_Not_Found);

    auto server = app->makeServer(router);
    server->listen("/tmp/fcgi.sock");
    app->exec();
    return 0;
}