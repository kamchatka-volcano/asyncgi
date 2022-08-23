#pragma once
#include "request.h"
#include "response.h"

namespace asyncgi{

template<class TRouteContext>
class RequestProcessor;

namespace detail{
class IRequestProcessor {
public:
    IRequestProcessor() = default;
    virtual ~IRequestProcessor() = default;
    IRequestProcessor(const IRequestProcessor&) = delete;
    IRequestProcessor& operator=(const IRequestProcessor&) = delete;
    IRequestProcessor(IRequestProcessor&&) = delete;
    IRequestProcessor&& operator=(IRequestProcessor&&) = delete;

    virtual void process(const Request&, ResponseContext&) = 0;

private:
    template<class TRouteContext>
    friend class asyncgi::RequestProcessor;
};
}

template <typename TRouteContext = detail::EmptyRouteContext>
class RequestProcessor : public detail::IRequestProcessor{
public:
    virtual void process(const Request&, Response<TRouteContext>&) = 0;

private:
    virtual void process(const Request& request, detail::ResponseContext& response) override
    {
        auto contextualResponse = Response<TRouteContext>{std::move(response)};
        process(request, contextualResponse);
    };
};

}
