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
    auto router = asyncgi::Router{io};
    router.route("/", http::RequestMethod::Get).process<GuestBookPage>(state);
    router.route("/", http::RequestMethod::Post).process<GuestBookAddMessage>(state);
    router.route(asyncgi::rx{"/delete/(.+)"}, http::RequestMethod::Post).process<GuestBookRemoveMessage>(state);
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