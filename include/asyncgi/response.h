#pragma once
#include <hot_teacup/response.h>
#include <memory>
#include <optional>

namespace asyncgi{
class Timer;
class RequestContext;

class Response{
public:

    template <typename TVal, std::enable_if_t<std::is_convertible_v<TVal, http::Response>>* = nullptr>
    void operator=(TVal&& val)
    {
        value_ = std::forward<TVal>(val);
    }

    template <typename TVal, std::enable_if_t<std::is_convertible_v<TVal, http::Response>>* = nullptr>
    void operator<<(TVal&& val)
    {
        value_ = std::forward<TVal>(val);
        send();
    }

    template <typename TVal>
    void set(TVal&& val)
    {
        value_ = std::forward<TVal>(val);
    }

    void send();
    Timer& timer();

private:
    Response(std::shared_ptr<RequestContext> context, Timer& timer);
    friend class Connection;

private:
    std::optional<http::Response> value_;
    std::shared_ptr<RequestContext> context_;
    std::reference_wrapper<Timer> timer_;
};

}
