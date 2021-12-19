#pragma once
#include "request.h"
#include "response.h"

namespace asyncgi{

template<class TResponseContext>
class RequestProcessor;

namespace detail{
class IRequestProcessor {
public:
    virtual void process(const Request&, Response&) = 0;

private:
    virtual ~IRequestProcessor() = default;

    template<class TResponseContext>
    friend class asyncgi::RequestProcessor;
};
}

template <typename TResponseContext = detail::EmptyContext>
class RequestProcessor : public detail::IRequestProcessor{
public:
    virtual void process(const Request&, Response<TResponseContext>&) = 0;

private:
    virtual void process(const Request& request, detail::Response& response) override
    {
        auto contextualResponse = Response<TResponseContext>{std::move(response)};
        process(request, contextualResponse);
    };
};


}
