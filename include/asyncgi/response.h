#pragma once
#include <hot_teacup/response.h>
#include <memory>
#include <optional>

namespace asyncgi{
class Timer;
class RequestContext;

class Response {
public:
    void send(const http::Response&);
    bool isSent() const;
    Timer& timer();

    Response(std::shared_ptr<RequestContext> context, Timer& timer);

private:
    std::shared_ptr<RequestContext> context_;
    std::reference_wrapper<Timer> timer_;
};

template <typename TResponseContext>
class ContextualResponse{
public:
    explicit ContextualResponse(Response response)
            : response_(std::move(response))
    {}

    TResponseContext& context()
    {
        return responseContext_;
    }

    void send(const http::Response& response)
    {
        response_.send(response);
    }

    bool isSent() const
    {
        return response_.isSent();
    }

    Timer& timer()
    {
        return response_.timer();
    }

private:
    Response response_;
    TResponseContext responseContext_;
};


}
