#pragma once

namespace asyncgi{
template<class TRouteContext, typename... TRouteParam>
class RequestProcessor;
class Request;
class ResponseContext;

namespace detail{

class IRequestProcessor {
public:
    IRequestProcessor() = default;
    virtual ~IRequestProcessor() = default;
    IRequestProcessor(const IRequestProcessor&) = delete;
    IRequestProcessor& operator=(const IRequestProcessor&) = delete;
    IRequestProcessor(IRequestProcessor&&) = delete;
    IRequestProcessor&& operator=(IRequestProcessor&&) = delete;

    virtual void processRequest(const Request&, ResponseContext&) = 0;

private:
    template<class TRouteContext, typename... TRouteParam>
    friend class asyncgi::RequestProcessor;
};

}
}