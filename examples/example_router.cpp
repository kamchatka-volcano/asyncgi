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

    http::Response operator()(const asyncgi::Request&)
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
        return page;
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

    http::Response operator()(const asyncgi::Request& request)
    {
        state_->addMessage(std::string{request.formField("msg")});
        return http::Redirect{"/"};
    }

private:
    GuestBookState* state_;
};

int main()
{
    auto io = asyncgi::IO{4}; //4 threads processing requests
    auto state = GuestBookState{};
    auto router = asyncgi::Router{io};
    router.route("/", http::RequestMethod::Get).process<GuestBookPage>(state);
    router.route("/", http::RequestMethod::Post).process<GuestBookAddMessage>(state);
    router.route().set(http::Response{http::ResponseStatus::_404_Not_Found, "Page not found"});
    //Alternatively, it's possible to pass arguments for creation of http::Response object to the set() method.
    //router.route().set(http::ResponseStatus::Code_404_Not_Found, "Page not found");

    auto server = asyncgi::Server{io, router};
#ifndef _WIN32
    server.listen("/tmp/fcgi.sock");
#else
    server.listen("127.0.0.1", 9088);
#endif
    io.run();
    return 0;
}